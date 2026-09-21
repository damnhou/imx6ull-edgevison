#!/usr/bin/env sh
set -eu
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(dirname "$script_dir")
build_dir="$project_root/build-imx6ull"
: "${BUILD_JOBS:=2}"

: "${CROSS_COMPILE:=arm-linux-gnueabihf-}"
export CROSS_COMPILE

mkdir -p "$build_dir"
cd "$build_dir"

cmake "$project_root" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="$project_root/cmake/imx6ull-gnueabihf.toolchain.cmake" \
  -DTHREADS_PTHREAD_ARG=2 \
  -DEDGEVISION_BUILD_TESTS=OFF
cmake --build . -- -j"$BUILD_JOBS"
"${CROSS_COMPILE}strip" "$build_dir/edgevision_cli"
file "$build_dir/edgevision_cli"
