#!/bin/bash -e

cd "$(dirname "$0")" || exit

source _base_env.sh

export PATH=$SCRIPTS_ROOT:$BUILD_ROOT/android-tools/build/vendor:$PATH

# rm -r $BUILD_ROOT/system/system/bin/

mkdir -p $BUILD_ROOT/system/system/bin/
mkdir -p $BUILD_ROOT/system/system/ext/

cp -r $PROJECT_ROOT/system/bin/* $BUILD_ROOT/system/system/bin/
cp -r $PROJECT_ROOT/system/ext/* $BUILD_ROOT/system/system/ext/

cd $BUILD_ROOT

arr=($(du -sb ./system))
size=${arr[0]}
echo "Total size in bytes: $size"

extra=$((100 * 1024 * 1024))

size=$((size + extra))

mkuserimg_mke2fs.py ./system/ system.img ext4 / $size --label /

avbtool.py add_hashtree_footer --image ./system.img --do_not_generate_fec --partition_name system

avbtool.py verify_image --image ./system.img

_notice_ "File: $(realpath ./system.img)"