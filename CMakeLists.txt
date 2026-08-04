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

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
)

# Dear ImGui has no CMakeLists.txt of its own, so this only fetches the
# source; the library target below builds it manually.
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.8
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3 glm imgui)

add_executable(lidar_viewer
    src/main.cpp
    src/FlyCamera.cpp
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

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(
    imgui
    PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# Use glad's already-loaded GL function pointers instead of the backend's
# bundled loader, since the app initializes glad itself before ImGui.
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_link_libraries(
    imgui
    PUBLIC
        SDL3::SDL3
        glad
)

find_package(OpenGL REQUIRED)

target_link_libraries(
    lidar_viewer
    PRIVATE
        SDL3::SDL3
        glad
        OpenGL::GL
        glm::glm
        imgui
)

# Bake the MinGW runtime into the exe so it runs on machines without MSYS2
# installed, instead of depending on libgcc/libstdc++/libwinpthread DLLs.
if(MINGW)
    target_link_options(lidar_viewer PRIVATE -static-libgcc -static-libstdc++ -static)
endif()


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

# Test data: Stanford Bunny range scans + reconstructed mesh, downloaded on
# demand instead of committed to the repo. https://graphics.stanford.edu/data/3Dscanrep/
set(BUNNY_DATA_DIR ${CMAKE_SOURCE_DIR}/TestData/StanfordBunnyData)

if(NOT EXISTS ${BUNNY_DATA_DIR}/bun_zipper.ply)
    message(STATUS "Downloading Stanford Bunny test data...")

    set(BUNNY_ARCHIVE ${CMAKE_BINARY_DIR}/bunny.tar.gz)
    file(DOWNLOAD
        https://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz
        ${BUNNY_ARCHIVE}
        EXPECTED_HASH SHA256=a5720bd96d158df403d153381b8411a727a1d73cff2f33dc9b212d6f75455b84
        SHOW_PROGRESS
    )

    file(ARCHIVE_EXTRACT
        INPUT ${BUNNY_ARCHIVE}
        DESTINATION ${CMAKE_BINARY_DIR}/_bunny_extract
    )

    file(MAKE_DIRECTORY ${BUNNY_DATA_DIR})
    file(GLOB BUNNY_FILES ${CMAKE_BINARY_DIR}/_bunny_extract/bunny/data/*)
    file(COPY ${BUNNY_FILES} DESTINATION ${BUNNY_DATA_DIR})

    file(GLOB BUNNY_RECON_FILES ${CMAKE_BINARY_DIR}/_bunny_extract/bunny/reconstruction/*)
    file(COPY ${BUNNY_RECON_FILES} DESTINATION ${BUNNY_DATA_DIR})
endif()

add_custom_command(
    TARGET lidar_viewer
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        ${BUNNY_DATA_DIR}/bun_zipper.ply
        $<TARGET_FILE_DIR:lidar_viewer>
    VERBATIM
)