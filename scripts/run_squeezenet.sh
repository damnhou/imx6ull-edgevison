#!/usr/bin/env sh
set -eu
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 IMAGE" >&2
    exit 2
fi
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(dirname "$script_dir")
"$project_root/build-imx6ull/edgevision_cli" \
  --param "$project_root/models/squeezenet_v1.1.param" \
  --bin "$project_root/models/squeezenet_v1.1.bin" \
  --labels "$project_root/models/synset_words.txt" \
  --image "$1" --width 227 --height 227 --pixel bgr \
  --mean 104,117,123 --norm 1,1,1 --input data --output prob --topk 5

