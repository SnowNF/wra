#!/bin/bash

cd "$(dirname "$0")" || exit

source _base_env.sh

mkdir -p $BUILD_ROOT

cd "$BUILD_ROOT" || exit 1

_notice_ "Creating directories"
set -x

mkdir -p system/acct
mkdir -p system/apex
mkdir -p system/bootstrap-apex
mkdir -p system/bt_firmware
mkdir -p system/cache
mkdir -p system/config
mkdir -p system/data
mkdir -p system/data_mirror
mkdir -p system/debug_ramdisk
mkdir -p system/dev
mkdir -p system/efs
mkdir -p system/firmware/radio
mkdir -p system/linkerconfig
mkdir -p system/metadata
mkdir -p system/mnt
mkdir -p system/odm
mkdir -p system/odm_dlkm
mkdir -p system/oem
mkdir -p system/persist
mkdir -p system/postinstall
mkdir -p system/proc
mkdir -p system/sec_storage
mkdir -p system/second_stage_resources
mkdir -p system/sys
mkdir -p system/system/bin
mkdir -p system/system/ext
mkdir -p system/system/lib64
mkdir -p system/system/product
mkdir -p system/system/system_ext/etc/init/config
mkdir -p system/system_dlkm
mkdir -p system/vendor
mkdir -p system/vendor_dlkm

set +x

_ln_() {
    local target="$1"
    local link_name="$2"

    if [ ! -e "$link_name" ]; then
        ln -sf "$target" "$link_name"
        echo "Created symlink: $link_name -> $target"
    else
        echo "Skipped: $link_name already exists"
    fi
}

_notice_ "Creating symlinks"
cd system || exit 1
_ln_ /sys/kernel/debug d

cd odm || exit 1
_ln_ /vendor/odm/app app
_ln_ /vendor/odm/bin bin
_ln_ /vendor/odm/etc etc
_ln_ /vendor/odm/firmware firmware
_ln_ /vendor/odm/framework framework
_ln_ /vendor/odm/lib lib
_ln_ /vendor/odm/lib64 lib64
_ln_ /vendor/odm/overlay overlay
_ln_ /vendor/odm/priv-app priv-app
_ln_ /vendor/odm/usr usr
cd ..

cd odm_dlkm || exit 1
_ln_ /odm/odm_dlkm/etc etc
cd ..

cd vendor_dlkm || exit 1
_ln_ /vendor/vendor_dlkm/etc etc
cd ..

_ln_ /system/bin bin
_ln_ /system/etc etc
_ln_ /system/bin/init init
_ln_ /system/product product
_ln_ /system/system_ext system_ext

_notice_ "Copying config"
set -x
cp -f $PROJECT_ROOT/system/skip_mount.cfg system/system_ext/etc/init/config
