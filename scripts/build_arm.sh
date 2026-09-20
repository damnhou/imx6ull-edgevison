#!/usr/bin/env sh
set -eu
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(dirname "$script_dir")
build_dir="$project_root/build-imx6ull"

: "${CROSS_COMPILE:=arm-linux-gnueabihf-}"
export CROSS_COMPILE

cmake -S "$project_root" -B "$build_dir" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="$project_root/cmake/imx6ull-gnueabihf.toolchain.cmake" \
  -DEDGEVISION_BUILD_TESTS=OFF
cmake --build "$build_dir" --parallel
"${CROSS_COMPILE}strip" "$build_dir/edgevision_cli"
file "$build_dir/edgevision_cli"

