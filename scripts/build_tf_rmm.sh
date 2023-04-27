#!/bin/bash
ROOT=$(git rev-parse --show-toplevel)
cd $ROOT/third-party/tf-rmm
pwd
# git submodule update --init --recursive
echo "XCOMPILE $1"
export CROSS_COMPILE=$1
CROSS_COMPILE=$1 cmake -DRMM_CONFIG=fvp_defcfg -S . -B build
CROSS_COMPILE=$1 cmake --build build

cp build/Release/rmm.img $ROOT/out/tf-rmm.img
