cmake_minimum_required(VERSION 3.14)
project(imx6ull_edgevision VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(EDGEVISION_BUILD_TESTS "Build dependency-free option parser tests" ON)
set(EDGEVISION_NCNN_SOURCE_DIR "" CACHE PATH "Path to a pinned ncnn source tree")

if(EDGEVISION_NCNN_SOURCE_DIR)
    set(_ncnn_source "${EDGEVISION_NCNN_SOURCE_DIR}")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ncnn/CMakeLists.txt")
    set(_ncnn_source "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ncnn")
endif()

if(_ncnn_source)
    set(NCNN_VULKAN OFF CACHE BOOL "" FORCE)
    set(NCNN_SHARED_LIB OFF CACHE BOOL "" FORCE)
    set(NCNN_OPENMP OFF CACHE BOOL "" FORCE)
    set(NCNN_SIMPLEOCV ON CACHE BOOL "" FORCE)
    set(NCNN_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(NCNN_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(NCNN_BUILD_BENCHMARK OFF CACHE BOOL "" FORCE)
    set(NCNN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(NCNN_INSTALL_SDK OFF CACHE BOOL "" FORCE)
    add_subdirectory("${_ncnn_source}" "${CMAKE_BINARY_DIR}/ncnn")
else()
    find_package(ncnn CONFIG REQUIRED)
endif()

add_executable(edgevision_cli
    src/main.cpp
    src/cli_options.cpp
    src/ncnn_classifier.cpp
    src/system_metrics.cpp
)
target_include_directories(edgevision_cli PRIVATE src)
target_compile_definitions(edgevision_cli PRIVATE USE_NCNN_SIMPLEOCV)

if(TARGET ncnn::ncnn)
    target_link_libraries(edgevision_cli PRIVATE ncnn::ncnn)
else()
    target_link_libraries(edgevision_cli PRIVATE ncnn)
endif()
if(WIN32)
    target_link_libraries(edgevision_cli PRIVATE psapi)
endif()

if(MSVC)
    target_compile_options(edgevision_cli PRIVATE /W4)
else()
    target_compile_options(edgevision_cli PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS edgevision_cli RUNTIME DESTINATION bin)

if(EDGEVISION_BUILD_TESTS)
    enable_testing()
    add_executable(cli_options_test tests/cli_options_test.cpp src/cli_options.cpp)
    target_include_directories(cli_options_test PRIVATE src)
    add_test(NAME cli_options_test COMMAND cli_options_test)
endif()
