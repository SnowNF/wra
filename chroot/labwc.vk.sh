#!/bin/bash

export WLR_RENDERER=vulkan
export MESA_LOADER_DRIVER_OVERRIDE=zink
unset LANGUAGE
unset LANG
unset LC_ALL

script_dir="$(dirname "$0")"
python3 "${script_dir}/labwc.py"