#!/bin/bash
mkdir -p ../out
export CROSS_COMPILE=arm-linux-gnueabihf-
make ARCH=arm O=../out merge_hi3630_defconfig
make ARCH=arm O=../out -j8
cp -f ../out/arch/arm/boot/zImage ../boot/
