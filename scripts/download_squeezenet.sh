#!/usr/bin/env sh
set -eu
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(dirname "$script_dir")
model_dir="$project_root/models"
mkdir -p "$model_dir"
curl -L --fail -o "$model_dir/squeezenet_v1.1.param" https://raw.githubusercontent.com/nihui/ncnn-assets/master/models/squeezenet_v1.1.param
curl -L --fail -o "$model_dir/squeezenet_v1.1.bin" https://raw.githubusercontent.com/nihui/ncnn-assets/master/models/squeezenet_v1.1.bin
curl -L --fail -o "$model_dir/synset_words.txt" https://raw.githubusercontent.com/Tencent/ncnn/20230816/examples/synset_words.txt
(cd "$model_dir" && sha256sum squeezenet_v1.1.param squeezenet_v1.1.bin synset_words.txt > checksums.sha256)

