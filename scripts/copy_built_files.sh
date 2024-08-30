#!/bin/bash

cd "$(dirname "$0")" || exit

source _base_env.sh

mkdir -p $BUILD_ROOT

cd $BUILD_ROOT || exit 1

_notice_ "Copying control-panel"
mkdir -p rootfs/opt/wra
cp -f rootfs-build/build/contorl-panel/control-panel rootfs/opt/wra

_notice_ "Copying turnip driver"
mkdir -p rootfs/usr/local/lib/aarch64-linux-gnu/
cp -f rootfs-build/usr/local/lib/aarch64-linux-gnu/libvulkan_freedreno.so \
    rootfs/usr/local/lib/aarch64-linux-gnu/libvulkan_freedreno.so

_notice_ "Copying turnip icd"
mkdir -p rootfs/usr/local/share/vulkan/icd.d/
cp rootfs-build/usr/local/share/vulkan/icd.d/freedreno_icd.aarch64.json \
    rootfs/usr/local/share/vulkan/icd.d/freedreno_icd.aarch64.json

_notice_ "Copying system binaries"
mkdir -p system/system/bin
cp -f rootfs-build/build/system/drm-test system/system/bin
cp -f rootfs-build/build/system/init system/system/bin
cp -f rootfs-build/build/system/ueventd system/system/bin
cp -f rootfs-build/build/system/wra-utils system/system/bin
cp -f rootfs-build/build/f2fs-tools/f2fs-tools-master/mkfs/mkfs.f2fs system/system/bin
cp -f rootfs-build/usr/bin/busybox system/system/bin

chmod -R 777 system/
