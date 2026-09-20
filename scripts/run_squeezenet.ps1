param(
    [Parameter(Mandatory=$true)][string]$Image,
    [string]$Executable = "$PSScriptRoot\..\build-host\edgevision_cli.exe"
)
$projectRoot = Split-Path -Parent $PSScriptRoot
& $Executable `
  --param "$projectRoot\models\squeezenet_v1.1.param" `
  --bin "$projectRoot\models\squeezenet_v1.1.bin" `
  --labels "$projectRoot\models\synset_words.txt" `
  --image $Image --width 227 --height 227 --pixel bgr `
  --mean 104,117,123 --norm 1,1,1 --input data --output prob --topk 5

