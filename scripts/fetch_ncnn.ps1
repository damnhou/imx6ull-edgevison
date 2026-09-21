param([string]$Tag = '20230816')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$destination = Join-Path $projectRoot 'third_party\ncnn'
if (Test-Path -LiteralPath $destination) {
    throw "ncnn already exists: $destination"
}
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
# Vulkan and Python bindings are disabled in this project, so NCNN submodules
# such as glslang and pybind11 are intentionally not downloaded.
git clone --depth 1 --branch $Tag https://github.com/Tencent/ncnn.git $destination
if ($LASTEXITCODE -ne 0) { throw 'Failed to clone ncnn' }
