cmake_minimum_required(VERSION 3.24)

project(LidarViewer
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.12
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)

add_executable(lidar_viewer
    src/main.cpp
)

add_library (
    glad
    external/glad/src/glad.c
)

target_include_directories(
    glad
    PUBLIC
        external/glad/include
)

find_package(OpenGL REQUIRED)

target_link_libraries(
    lidar_viewer
    PRIVATE
        SDL3::SDL3
        glad
        OpenGL::GL
)


if(WIN32)
    add_custom_command(
        TARGET lidar_viewer
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:lidar_viewer>
        VERBATIM
    )
endif()