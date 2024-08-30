#!/bin/bash

cd "$(dirname "$0")" || exit

set -e
set -x

sudo ./prepare_build_env.sh

sudo ./build_container_files.sh

sudo ./build_system_files.sh

sudo ./copy_built_files.sh

sudo ./configure_rootfs.sh

sudo ./packaging_rootfs.sh

./setup_system_structure.sh

sudo ./extract_gsi.sh

./packaging_system.sh