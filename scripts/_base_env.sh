#!/bin/bash

PROJECT_ROOT=./../
PROJECT_ROOT=$(realpath "$PROJECT_ROOT")
# shellcheck disable=SC2034
BUILD_ROOT=$PROJECT_ROOT/build
# shellcheck disable=SC2034
SCRIPTS_ROOT=$PROJECT_ROOT/scripts

# set -x
set -e

_notice_() {
    echo "########################"
    echo "$@"
    echo "########################"
}