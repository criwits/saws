cmake_minimum_required(VERSION 3.21)
project(saws VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/**/*.c"
        "${PROJECT_SOURCE_DIR}/src/*.c")

set(HEADER_DIRECTORIES
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/include/common")

include_directories(${HEADER_DIRECTORIES})
add_executable(saws ${SOURCE_FILES})

set(CMAKE_BUILD_TYPE DEBUG)
set(LWS_WITH_ASAN 1)

target_link_libraries(saws websockets mariadb pthread)