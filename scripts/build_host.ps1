$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
cmake -S $projectRoot -B (Join-Path $projectRoot 'build-host') -DCMAKE_BUILD_TYPE=Release
cmake --build (Join-Path $projectRoot 'build-host') --config Release --parallel
Push-Location (Join-Path $projectRoot 'build-host')
try {
    ctest -C Release --output-on-failure
} finally {
    Pop-Location
}
