#!/usr/bin/env sh
set -eu
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 root@BOARD_IP" >&2
    exit 2
fi
target="$1"
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(dirname "$script_dir")
ssh "$target" 'mkdir -p /opt/edgevision/models /opt/edgevision/assets'
scp "$project_root/build-imx6ull/edgevision_cli" "$target:/opt/edgevision/"
scp "$project_root/models/squeezenet_v1.1.param" "$project_root/models/squeezenet_v1.1.bin" \
    "$project_root/models/synset_words.txt" "$target:/opt/edgevision/models/"

