# LiDAR-Viewer

![LiDAR Viewer demo](lidar-viewer.gif)

A lightweight C++ desktop viewer for sequential LiDAR point cloud datasets. Load a folder of KITTI or nuScenes `.bin` frames, step or autoplay through them at the dataset's native frame rate, and fly around the scene with intensity-colored points, a reference grid, and XYZ axes.

Built with [SDL3](https://github.com/libsdl-org/SDL), OpenGL 4.1 (core profile), [Dear ImGui](https://github.com/ocornut/imgui), and [GLM](https://github.com/g-truc/glm).

## Features

- Load a single `.bin` frame or an entire folder of sequential frames
- KITTI / nuScenes format switch (handles each format's point layout and sensor mount height automatically)
- Playback controls: jump to start/end, step frame-by-frame, play/pause, autoplay at the dataset's real frame rate (10Hz KITTI, 20Hz nuScenes sweeps)
- Intensity-based colormap (percentile-normalized, so outlier returns don't wash out the color range)
- Reference ground grid + colored XYZ axes
- Free-fly camera (WASD + mouse look)

## Requirements

- CMake 3.24+
- A C++20 compiler (developed and tested with MinGW-w64 g++ on Windows)
- An OpenGL 4.1–capable GPU/driver

All other dependencies (SDL3, GLM, Dear ImGui, IconFontCppHeaders) are fetched automatically by CMake — nothing to install manually for those.

**One manual step**: the toolbar icons use Font Awesome's free desktop font, which isn't redistributed here. Download the [Font Awesome Free desktop package](https://fontawesome.com/download) and place `fa-solid-900.ttf` at `src/fonts/fa-solid-900.ttf` before building. The app still builds and runs without it — the icon buttons just won't render.

## Build & Run

Any CMake generator should work; these commands use Ninja, which is what this project is built and tested with:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/lidar_viewer.exe
```

Run the executable from inside its build directory (or copy the whole build output folder if you move it elsewhere) — shaders, the icon font, and test data are loaded via paths relative to the executable, not embedded in it.

## Controls

| Action | Input |
|---|---|
| Open a single frame | "Open File" button |
| Open a folder of frames | "Open Folder" button |
| Switch dataset format | Format dropdown (top bar) |
| Jump to first / last frame | ⏮⏮ / ⏭⏭ buttons |
| Step one frame back / forward | ⏮ / ⏭ buttons, or **Left** / **Right** arrow keys |
| Play / pause autoplay | ▶ / ⏸ button, or **Space** |
| Toggle camera mode | **Tab** |
| Move camera (camera mode only) | **W A S D** |
| Look around (camera mode only) | Mouse |
| Quit | **Esc** |

## Datasets

Tested against:
- [KITTI raw data](https://www.cvlibs.net/datasets/kitti/raw_data.php) — point "Open Folder" at a sequence's `velodyne_points/data` folder.
- [nuScenes](https://www.nuscenes.org/download) — point it at `samples/LIDAR_TOP` (annotated keyframes, 2Hz) or `sweeps/LIDAR_TOP` (the sensor's native 20Hz rate).

Datasets are not included in this repository — see the links above to download them yourself.
