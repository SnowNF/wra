#!/bin/bash
set -x
set -e

apt-get install -y busybox-static

command -v wget &>/dev/null || apt-get install -y wget

apt-get install -y libblkid-dev libselinux1-dev pkg-config uuid-dev automake libtool

cd /build || exit 1
mkdir -p f2fs-tools
cd f2fs-tools || exit 1
if [ ! -f "f2fs-tools-master.zip" ]; then
    wget https://salsa.debian.org/debian/f2fs-tools/-/archive/master/f2fs-tools-master.zip
fi

rm -rf f2fs-tools-master

busybox unzip f2fs-tools-master.zip

cd f2fs-tools-master
./autogen.sh

echo "mkfs_f2fs_LDFLAGS = -all-static" >>mkfs/Makefile.am
echo "fsck_f2fs_LDFLAGS = -all-static" >>fsck/Makefile.am

LDFLAGS=--static ./configure --without-selinux

make -j
