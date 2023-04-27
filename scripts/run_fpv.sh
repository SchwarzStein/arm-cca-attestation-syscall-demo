#!/bin/bash
#

args = ["./FVP_Base_RevC-2xAEMvA",
        "-C", "bp.flashloader0.fname=%s/fip.bin" % OUT,
        "-C", "bp.secureflashloader.fname=%s/bl1.bin" % OUT,
        "-C", "bp.virtioblockdevice.image_path=%s/boot.img" % OUT,
        "-C", "bp.virtiop9device.root_path=%s" % PC_SHARE_DIR,
        "-C", "bp.virtio_net.hostbridge.userNetworking=true",
        "-C", "bp.virtio_net.enabled=true",
        "-C", "bp.virtio_net.hostbridge.userNetPorts=1234=1234",
        "-f", CONFIG,
        "-Q", "1000"]
