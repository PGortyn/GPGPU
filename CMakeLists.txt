cmake_minimum_required(VERSION 3.20)

project(GPGPU)

find_package(OpenCL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    GPGPU
    src/main.cpp
)

target_include_directories(
    GPGPU
    PRIVATE include
)

target_include_directories(
	GPGPU 
	PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/stb_image
)

target_link_libraries(
    GPGPU
    OpenCL::OpenCL
)

file(GLOB_RECURSE ASSETS_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/res/*.*)

target_sources(GPGPU PRIVATE ${ASSETS_FILES})

source_group(TREE ${CMAKE_SOURCE_DIR} PREFIX "Resources" FILES ${ASSETS_FILES})

add_custom_command(TARGET GPGPU POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:GPGPU>/res
)

file(GLOB STB_HEADERS
    ${CMAKE_SOURCE_DIR}/thirdparty/stb_image/*.h
)

source_group(TREE ${CMAKE_SOURCE_DIR}/thirdparty/stb_image
    PREFIX "ThirdParty\\stb"
    FILES ${STB_HEADERS}
)
