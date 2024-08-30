#!/bin/bash

cd "$(dirname "$0")" || exit

source _base_env.sh

mkdir -p $BUILD_ROOT

cd $BUILD_ROOT

do_main() {
    mkdir -p rootfs-build/build
    _notice_ "Installing sudo in rootfs"
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "apt-get update"
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "command -v sudo || apt-get install sudo -y"
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "cd /build&&bash -e ../container/build-turnip-driver.sh"
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "cd /build&&bash -e ../container/build-control-panel.sh"
}

mkdir -p rootfs-build/container
mount --bind $PROJECT_ROOT/container rootfs-build/container
mount --bind /proc rootfs-build/proc
mount --bind /dev rootfs-build/dev/
mount -t devpts devpts rootfs-build/dev/pts
_notice_ "Building container files"
do_main || true
umount rootfs-build/container
umount rootfs-build/proc
umount rootfs-build/dev/pts
umount rootfs-build/dev/