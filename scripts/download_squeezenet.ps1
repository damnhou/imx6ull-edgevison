$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$modelDir = Join-Path $projectRoot 'models'
New-Item -ItemType Directory -Force -Path $modelDir | Out-Null
$files = @{
    'squeezenet_v1.1.param' = 'https://raw.githubusercontent.com/nihui/ncnn-assets/master/models/squeezenet_v1.1.param'
    'squeezenet_v1.1.bin' = 'https://raw.githubusercontent.com/nihui/ncnn-assets/master/models/squeezenet_v1.1.bin'
    'synset_words.txt' = 'https://raw.githubusercontent.com/Tencent/ncnn/20230816/examples/synset_words.txt'
}
foreach ($name in $files.Keys) {
    Invoke-WebRequest -Uri $files[$name] -OutFile (Join-Path $modelDir $name)
}
Get-FileHash (Join-Path $modelDir 'squeezenet_v1.1.param'), (Join-Path $modelDir 'squeezenet_v1.1.bin'), (Join-Path $modelDir 'synset_words.txt') -Algorithm SHA256 |
    ForEach-Object { "$($_.Hash.ToLower())  $(Split-Path -Leaf $_.Path)" } |
    Set-Content -Encoding ascii (Join-Path $modelDir 'checksums.sha256')

