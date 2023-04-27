#!/usr/bin/bash
ROOT=$(pwd)
WORKDIR=$ROOT/out/pc_share_dir/guest
# rootfs_cpio_old = $WORKDIR/rootfs-realm.cpio
rootfs_flat_dir=$WORKDIR/rootfs
# call_bin=~/attest_user/call
call_bin=$ROOT/attestation-demo/call
server_bin=$ROOT/attestation-demo/server
client_bin=$ROOT/attestation-demo/client

rm -rf ./out/pc_share_dir/guest/rootfs
mkdir -p ./out/pc_share_dir/guest/rootfs
cd ./out/pc_share_dir/guest/
gzip -d rootfs-realm.cpio.gz
cd rootfs
cpio -idm < ../rootfs-realm.cpio

cp $call_bin $rootfs_flat_dir/call
cp $client_bin $rootfs_flat_dir/client
cp $server_bin $rootfs_flat_dir/server
find . | cpio --create --format='newc' > ../rootfs-realm.cpio
cd ..
gzip -f rootfs-realm.cpio
