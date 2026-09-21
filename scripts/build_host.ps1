param([string]$QtMinGwBin = '')

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $projectRoot 'build-host-mingw64'

$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakeCommand) {
    $cmakeExe = $cmakeCommand.Source
} elseif (Test-Path -LiteralPath 'C:\Program Files\CMake\bin\cmake.exe') {
    $cmakeExe = 'C:\Program Files\CMake\bin\cmake.exe'
} else {
    throw 'CMake was not found. Install Kitware.CMake first.'
}

if (-not $QtMinGwBin) {
    $toolRoots = @('C:\Qt\Tools', 'D:\Qt\Tools', 'F:\Qt\Tools')
    $QtMinGwBin = $toolRoots |
        Where-Object { Test-Path -LiteralPath $_ } |
        ForEach-Object { Get-ChildItem -LiteralPath $_ -Directory -Filter 'mingw*_64' } |
        Sort-Object Name -Descending |
        ForEach-Object { Join-Path $_.FullName 'bin' } |
        Where-Object { Test-Path -LiteralPath (Join-Path $_ 'g++.exe') } |
        Select-Object -First 1
}
if (-not $QtMinGwBin) {
    throw 'A 64-bit Qt MinGW toolchain was not found. Pass -QtMinGwBin explicitly.'
}

$env:PATH = "$QtMinGwBin;$env:PATH"
$configureArgs = @(
    '-S', $projectRoot,
    '-B', $buildDir,
    '-G', 'MinGW Makefiles',
    '-DCMAKE_BUILD_TYPE=Release',
    '-DCMAKE_POLICY_VERSION_MINIMUM=3.5',
    '-DNCNN_AVX512=OFF',
    "-DCMAKE_C_COMPILER=$(Join-Path $QtMinGwBin 'gcc.exe')",
    "-DCMAKE_CXX_COMPILER=$(Join-Path $QtMinGwBin 'g++.exe')",
    "-DCMAKE_MAKE_PROGRAM=$(Join-Path $QtMinGwBin 'mingw32-make.exe')"
)
& $cmakeExe @configureArgs
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
& $cmakeExe --build $buildDir --parallel 4
if ($LASTEXITCODE -ne 0) { throw 'CMake build failed.' }

$ctestExe = Join-Path (Split-Path -Parent $cmakeExe) 'ctest.exe'
Push-Location $buildDir
try {
    & $ctestExe --output-on-failure
    if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
} finally {
    Pop-Location
}
