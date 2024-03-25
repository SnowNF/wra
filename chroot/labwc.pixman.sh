#!/bin/bash

export WLR_RENDERER=pixman

script_dir="$(dirname "$0")"
python3 "${script_dir}/labwc.py"
