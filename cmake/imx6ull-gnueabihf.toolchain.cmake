set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(DEFINED ENV{CROSS_COMPILE} AND NOT "$ENV{CROSS_COMPILE}" STREQUAL "")
    set(_cross "$ENV{CROSS_COMPILE}")
else()
    set(_cross "arm-linux-gnueabihf-")
endif()

set(CMAKE_C_COMPILER "${_cross}gcc")
set(CMAKE_CXX_COMPILER "${_cross}g++")
set(CMAKE_AR "${_cross}ar")
set(CMAKE_RANLIB "${_cross}ranlib")
set(CMAKE_STRIP "${_cross}strip")

if(DEFINED ENV{IMX6ULL_SYSROOT} AND NOT "$ENV{IMX6ULL_SYSROOT}" STREQUAL "")
    set(CMAKE_SYSROOT "$ENV{IMX6ULL_SYSROOT}")
    set(CMAKE_FIND_ROOT_PATH "$ENV{IMX6ULL_SYSROOT}")
endif()

set(_cpu_flags "-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard")
set(CMAKE_C_FLAGS_INIT "${_cpu_flags}")
set(CMAKE_CXX_FLAGS_INIT "${_cpu_flags}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

