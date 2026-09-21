set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(DEFINED ENV{CROSS_COMPILE} AND NOT "$ENV{CROSS_COMPILE}" STREQUAL "")
    set(_cross "$ENV{CROSS_COMPILE}")
else()
    set(_cross "arm-linux-gnueabihf-")
endif()

find_program(_cross_cc NAMES "${_cross}gcc")
find_program(_cross_cxx NAMES "${_cross}g++")
find_program(_cross_ar NAMES "${_cross}ar")
find_program(_cross_ranlib NAMES "${_cross}ranlib")
find_program(_cross_strip NAMES "${_cross}strip")

if(NOT _cross_cc OR NOT _cross_cxx OR NOT _cross_ar OR NOT _cross_ranlib)
    message(FATAL_ERROR "Incomplete cross toolchain for prefix: ${_cross}")
endif()

set(CMAKE_C_COMPILER "${_cross_cc}")
set(CMAKE_CXX_COMPILER "${_cross_cxx}")
set(CMAKE_AR "${_cross_ar}" CACHE FILEPATH "Cross archiver" FORCE)
set(CMAKE_RANLIB "${_cross_ranlib}" CACHE FILEPATH "Cross ranlib" FORCE)
set(CMAKE_STRIP "${_cross_strip}" CACHE FILEPATH "Cross strip" FORCE)

if(DEFINED ENV{IMX6ULL_SYSROOT} AND NOT "$ENV{IMX6ULL_SYSROOT}" STREQUAL "")
    set(CMAKE_SYSROOT "$ENV{IMX6ULL_SYSROOT}")
    set(CMAKE_FIND_ROOT_PATH "$ENV{IMX6ULL_SYSROOT}")
endif()

set(_cpu_flags "-march=armv7ve -mcpu=cortex-a7 -mfpu=neon -mfloat-abi=hard")
# CMake 3.5 from the vendor Ubuntu VM does not reliably propagate the
# *_FLAGS_INIT variables during its compiler probe.  Cache the ABI flags so
# both compilation and the initial link test use the hard-float ABI.
set(CMAKE_C_FLAGS "${_cpu_flags}" CACHE STRING "C flags for i.MX6ULL" FORCE)
set(CMAKE_CXX_FLAGS "${_cpu_flags}" CACHE STRING "C++ flags for i.MX6ULL" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
