#!/bin/bash
#
ROOT=$(pwd)
OUT=$ROOT/out
SHARE_DIR=${OUT}/pc_share_dir

./assets/fastmodel/Base_RevC_AEMvA_pkg/models/Linux64_GCC-9.3/FVP_Base_RevC-2xAEMvA \
        -C bp.flashloader0.fname=$OUT/fip.bin \
        -C bp.secureflashloader.fname=$OUT/bl1.bin \
        -C bp.virtioblockdevice.image_path=$OUT/boot.img \
        -C bp.virtiop9device.root_path=$SHARE_DIR \
        -C bp.virtio_net.hostbridge.userNetworking=true \
        -C bp.virtio_net.enabled=true \
        -C bp.virtio_net.hostbridge.userNetPorts=1234=1234 \
        -f ${ROOT}/scripts/fvp_config \
        -Q 1000 \

