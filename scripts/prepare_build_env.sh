#!/bin/bash

cd "$(dirname "$0")" || exit

source _base_env.sh

mkdir -p $BUILD_ROOT

cd "$BUILD_ROOT" || exit 1

if [ ! -d "rootfs-build" ]; then
    _notice_ "Bootstraping Debian sid...in "$(pwd)"/rootfs-build for build"
    debootstrap --arch=arm64 sid rootfs-build
else
    echo $(pwd)"/rootfs-build already exists,skiping"
fi

if [ ! -d "rootfs" ]; then
    _notice_ "Copy Debian sid...to "$(pwd)"/rootfs for run"
    cp -r rootfs-build rootfs
else
    echo $(pwd)"/rootfs already exists,skiping"
fi

_notice_ "Building andoroid-tools"

if [ ! -f "android-tools-35.0.1.tar.xz" ]; then
    _notice_ "Downloading android-tools source"
    wget https://github.com/nmeum/android-tools/releases/download/35.0.1/android-tools-35.0.1.tar.xz
fi

if [ ! -d "android-tools" ]; then
    _notice_ "Decompressing android-tools source"
    tar xf android-tools-35.0.1.tar.xz
    mv android-tools-35.0.1 android-tools
fi

_notice_ "Building android-tools"
mkdir -p android-tools/build
cd android-tools/build || exit 1
apt install golang-go liblz4-dev libusb-1.0-0-dev libprotobuf-dev libunwind-dev protobuf-compiler
cmake ..
cmake --build . -j
