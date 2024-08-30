#!/bin/bash

cd "$(dirname "$0")" || exit

source _base_env.sh

mkdir -p "$BUILD_ROOT"

cd "$BUILD_ROOT" || exit 1

main() {
    mkdir -p rootfs-build/build
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "apt-get update"
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "mkdir -p /build/system && cd /build/system && cmake /ext/system/src -DCMAKE_BUILD_TYPE=Release"
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "cd /build/system && cmake --build . -j"
    chroot rootfs-build /bin/su -l root -c /bin/bash -c "/ext/system/build-tools.sh"
}

mkdir -p rootfs-build/ext/system
mount --bind /proc rootfs-build/proc
mount --bind "$PROJECT_ROOT"/system rootfs-build/ext/system
main || true
umount rootfs-build/ext/system
umount rootfs-build/proc