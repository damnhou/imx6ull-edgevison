#!/usr/bin/env sh
set -eu
tag="${1:-20230816}"
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(dirname "$script_dir")
destination="$project_root/third_party/ncnn"
if [ -e "$destination" ]; then
    echo "ncnn already exists: $destination" >&2
    exit 1
fi
mkdir -p "$(dirname "$destination")"
# Vulkan and Python bindings are disabled, so NCNN submodules are unnecessary.
git clone --depth 1 --branch "$tag" https://github.com/Tencent/ncnn.git "$destination"
