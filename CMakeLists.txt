cmake_minimum_required(VERSION 3.20)

project(GPGPU)

find_package(OpenCL REQUIRED)

add_executable(
    GPGPU
    src/main.cpp
)

target_include_directories(
    GPGPU
    PRIVATE include
)

target_link_libraries(
    GPGPU
    OpenCL::OpenCL
)