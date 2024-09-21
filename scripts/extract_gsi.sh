#!/bin/bash -e

cd "$(dirname "$0")" || exit

source _base_env.sh

cd $BUILD_ROOT

if [ ! -f "gsi.img" ]; then
    echo "Missing $(pwd)/gsi.img"
    exit 1
fi

do_copy() {
    rm -rf system/system/lib64
    cp -rf gsi/system/lib64 system/system/
    cp -f gsi/system/bin/bootstrap/linker64 system/system/bin/
}

mkdir -p gsi
_notice_ "Mounting gsi.img"
mount --read-only ./gsi.img ./gsi
do_copy || true
_notice_ "Umounting gsi.img"
umount ./gsi
