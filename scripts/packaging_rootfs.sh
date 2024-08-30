#!/bin/bash -e

cd "$(dirname "$0")" || exit

source _base_env.sh

cd $BUILD_ROOT

echo "Clean Rootfs..."
rm -r rootfs/var/cache/apt/ | true
rm -r rootfs/var/log/ | true
rm -r rootfs/var/lib/apt/lists/ | true

echo "Compressing..."
tar -cf - rootfs | pigz -p "$(nproc)" > system/system/rootfs.tgz
chmod -R 777 system/
