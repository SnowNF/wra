#!/bin/bash
set -x
set -e

apt-get install -y qt6-base-dev cmake

cd /build || exit 1
mkdir -p contorl-panel
cd contorl-panel || exit 1
cmake /container/control-panel -DCMAKE_BUILD_TYPE=Release
cmake --build . -j