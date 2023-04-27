ROOT = $(shell pwd)
ARCH = arm64
OUT = $(ROOT)/out
MAKE_PID := $(shell echo $$PPID)
JOB_FLAG := $(filter -j%, $(subst -j ,-j,$(shell ps T | grep "^\s*$(MAKE_PID).*$(MAKE)")))
JOBS     := $(subst -j,,$(JOB_FLAG))
# JOBS     := 40
# -s or none
MAKEQUIET = -s

# PREBUILTS
PREBUILT = $(ROOT)/assets/prebuilt
QEMU_PREBUILT = $(PREBUILT)/qemu/qemu-system-aarch64
GRUB_PREBUILT = $(PREBUILT)/bootaa64.efi
EDK2_PREBUILT = $(PREBUILT)/FVP_AARCH64_EFI.fd

# ISLET UPSTREAM
# We use their fork of optee-build
ISLET_UPSTREAM = https://github.com/Samsung/islet

# THIRD-PARTIES
THIRD-PARTY_FROM_SOURCE = kvmtool optee-build linux-cca

# COMMITS .. NOW WE MANUALLY TRACK THESE.. 
COMMIT_TF-RMM = 708eb9d6ef16700c38aa89232e63e8bdea18c486
COMMIT_OPTEE-BUILD = ca9f1655342f
COMMIT_TF-A = 33b4df9c1f77

# FOR TF-RMM
TF-RMM = third-party/tf-rmm
RMM_CROSS_COMPILE = $(ROOT)/assets/toolchain/aarch64-none-elf/bin/aarch64-none-elf- 
DTC = $(ROOT)/assets/dtc

# FOR KVMTOOL
KVMTOOL = third-party/kvmtool
KVMTOOL_CROSS_COMPILE = $(ROOT)/assets/toolchain/aarch64-none-linux-gnu-10-2/bin/aarch64-none-linux-gnu-
LIBFDT = $(ROOT)/assets/dtc/libfdt

# FOR OPTEE-BUILD
OPTEE-BUILD = third-party/optee-build

# FOR NW-LINUX
NW-LINUX = third-party/nw-linux

# FOR THE BOOTLOADER
TF-A = third-party/tf-a
TF-A-ARGS = CROSS_COMPILE=$(RMM_CROSS_COMPILE) PLAT=fvp ENABLE_RME=1 \
			FVP_HW_CONFIG_DTS=fdts/fvp-base-gicv3-psci-1t.dts \
			DEBUG=1 BL33=$(EDK2_PREBUILT) RMM=$(OUT)/tf-rmm.img \
			all fip

# FOR REALM
REALM-LINUX = third-party/realm-linux
REALM_ROOTFS = assets/rootfs
GUEST_SHARED = $(OUT)/pc_share_dir/guest
LAUNCH_REALM = $(ROOT)/scripts/launch_realm_on_fvp.sh

# LINUX-CCA
LINUX-CCA = third-party/linux-cca

all: realm bootloader nw-linux


init: third-party-source assets
	@mkdir -p $(OUT)/out
	@git submodule update --init --recursive
	@git -C $(TF-RMM) checkout $(COMMIT_TF-RMM)
	@git -C $(TF-A) checkout $(COMMIT_TF-A)

assets: unzip_assets

unzip_assets: 
	# @tar -zxvf assets.tar.gz

# this is optional
realm: realm_guest_shared

realm_guest_shared: kvmtool realm-linux attestation-demo
	@mkdir -p $(GUEST_SHARED)/
	@cp -R $(REALM_ROOTFS)/rootfs-realm.cpio.gz $(GUEST_SHARED)/
	@./add_demo_to_rootfs.sh
	@cp -R $(OUT)/realm/. $(GUEST_SHARED)/
	@cp $(OUT)/lkvm $(GUEST_SHARED)/
	@cp $(LAUNCH_REALM) $(GUEST_SHARED)/


realm-linux: third-party/realm-linux
	@make $(MAKEQUIET) -f ./realm-linux.mk
	@make $(MAKEQUIET) -f ./realm-linux.mk install

.PHONY: attestation-demo
attestation-demo:
	@make -C attestation-demo call server client

bootloader: tf-a
	@cp $(TF-A)/build/fvp/debug/bl1.bin $(OUT)/
	@cp $(TF-A)/build/fvp/debug/fip.bin $(OUT)/

# tf-a's makefile is buggy here, i.e. it
# won't skip the already-compiled targets
# not sure if i should put nw-linux here but it
# seems so..
tf-a: init rmm nw-linux
	@echo "BUILDING  	--  	tf-a"
	@make $(MAKEQUIET) -C $(TF-A) $(TF-A-ARGS)

rmm: init
	# TODO maybe put .sh content here
	@echo "BUILDING  	--  	tf-rmm"
	@./scripts/build_tf_rmm.sh $(RMM_CROSS_COMPILE)

kvmtool: third-party/kvmtool init
	@echo "BUILDING  	--  	kvmtool"
	@make $(MAKEQUIET) -j $(JOBS) -C $(KVMTOOL) CROSS_COMPILE=$(KVMTOOL_CROSS_COMPILE) \
		ARCH=$(ARCH) LIBFDT_DIR=$(LIBFDT) lkvm
	@cp $(KVMTOOL)/lkvm $(OUT)/

nw-linux: third-party/optee-build third-party/nw-linux init
	@echo "BUILDING 	-- 		nw-linux"
	@make -j$(JOBS) -C $(OPTEE-BUILD) -f fvp.mk linux
	@cp $(NW-LINUX)/arch/arm64/boot/Image $(OUT)/
	@cp $(NW-LINUX)/arch/arm64/boot/dts/arm/fvp-base-revc.dtb $(OUT)/
	@cp $(GRUB_PREBUILT) $(OUT)/
	@make -j$(JOBS) -C $(OPTEE-BUILD) -f fvp.mk boot-img

third-party-source: $(addprefix third-party/,$(THIRD-PARTY_FROM_SOURCE))

third-party/kvmtool:
	@echo "FETCHING SOURCE CODE FOR -- kvmtool"
	@-git clone https://git.gitlab.arm.com/linux-arm/kvmtool-cca.git ${KVMTOOL}
	@-git -C ${KVMTOOL} checkout cca/rfc-v1

.PHONY: third-party/nw-linux
third-party/nw-linux: third-party/linux-cca
	@echo "FETCHING SOURCE CODE FOR -- nw-linux"
	-rm -f third-party/nw-linux
	-ln -s $(ROOT)/third-party/linux-cca $(ROOT)/third-party/nw-linux
	-git -C $(NW-LINUX) checkout cca-full/rfc-v1
	-git -C $(REALM-LINUX) checkout -b host
	-git -C $(NW-LINUX) am $(ROOT)/patch/linux_host_deconfig.patch

.PHONY: third-party/realm-linux
third-party/realm-linux: third-party/linux-cca
	@echo "FETCHING SOURCE CODE FOR -- realm-linux"
	-rm -f  third-party/realm-linux
	-ln -s $(ROOT)/third-party/linux-cca $(ROOT)/third-party/realm-linux
	-git -C $(REALM-LINUX) checkout cca-full/rfc-v1
	-git -C $(REALM-LINUX) checkout -b realm
	-git -C $(REALM-LINUX) am $(ROOT)/patch/attest_syscall.patch

third-party/optee-build:
	@echo "FETCHING SOURCE CODE FOR -- optee-build"
	@./scripts/deps/fetch_thirdparty.sh $(ISLET_UPSTREAM) optee-build $(COMMIT_OPTEE-BUILD) 

third-party/linux-cca:
	@echo "FETCHING SOURCE CODE FOR -- linux-cca"
	-git clone https://gitlab.arm.com/linux-arm/linux-cca $(LINUX-CCA)
	-git -C $(linux-cca) checkout cca-full/rfc-v1

clean:
	@make -C $(TF-A) distclean 
	@make -C $(REALM-LINUX) distclean
	@make -C $(KVMTOOL) clean
	@make -C $(OPTEE-BUILD) -f fvp.mk linux-clean boot-img-clean
	@rm -rf $(TF-RMM)/build
	@rm -rf out/*

clean-third-party:
	rm -rf $(addprefix third-party/,$(THIRD-PARTY_FROM_SOURCE))
	@rm -rf third-party/*.zip

.PHONY: all clean third-party init

	
