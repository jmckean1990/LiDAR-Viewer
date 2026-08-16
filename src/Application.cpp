#include "Application.hpp"
#include "FlyCamera.hpp"
#include "Renderer.hpp"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "IconsFontAwesome6.h"

#include <iostream>
#include <random>
#include <fstream>
#include <algorithm>
#include <vector>
#include <filesystem>



Application::Application() {
    init();
}

bool Application::init() {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL3 Could not initialize video subsystem" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    // set OpenGL major and minor versions
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Setup for disabling old OpenGL functionality
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // turn on double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // set depth of buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    graphicsApplicationWindow = SDL_CreateWindow(
                                "OpenGL Window", 
                                screenWidth, screenHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(graphicsApplicationWindow == nullptr) {
        std::cout << "SDL Window could not be created." << std::endl;
        return false;
    }

    SDL_SetWindowRelativeMouseMode(graphicsApplicationWindow, false);

    openGLContext = SDL_GL_CreateContext(graphicsApplicationWindow);

    if(openGLContext == nullptr) {
        std::cout << "SDL Context not available." << std::endl;
        return false;
    }

    // enable vsync
    SDL_GL_SetSwapInterval(1);

    // initialize the Glad Library
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "Glad was not initialized." << std::endl;
        return false;
    }

    renderer.emplace("VertexShader.vert", "FragmentShader.frag");


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig baseConfig;
    baseConfig.SizePixels = 16.0f;
    io.Fonts->AddFontDefault(&baseConfig);
    
    // io.Fonts->AddFontDefault();
    static const ImWchar iconRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig iconConfig;
    iconConfig.MergeMode = true;
    iconConfig.PixelSnapH = true;
    iconConfig.GlyphMinAdvanceX = 16.0f;
    io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", 16.0f, &iconConfig, iconRanges);

    // TODO: NEED A FIX FOR THIS
    // io.IniFilename = nullptr;       // temporarily disable permenant setup docking chagnes for dev
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL3_InitForOpenGL(graphicsApplicationWindow, openGLContext);
    ImGui_ImplOpenGL3_Init("#version 410");

    return true;
}

void Application::run() {    
    float lastFrame {0.0f};
    ImVec2 iconSize(24, 24);
    float frameChangeInterval = 1.0f / formatInfo(currentFormat).frameInterval;   // KITTI frames taken at 10Hz

    while(!running) {
        Uint64 ticks {SDL_GetTicks()};
        float timeSeconds {ticks / 1000.0f};
        float deltaTime = {timeSeconds - lastFrame};

        lastFrame = timeSeconds;

        running = input(deltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::BeginMainMenuBar();
        if(ImGui::Button(ICON_FA_FILE " Open File", iconSize)) {
            std::cout << "Open file pressed" << "\n";
            SDL_ShowOpenFileDialog(openFileCallback, this, graphicsApplicationWindow, NULL, 0, NULL, false);
        }
        if(ImGui::Button(ICON_FA_FOLDER_OPEN " Open Folder", iconSize)) {
            isPlaying = false;
            SDL_ShowOpenFolderDialog(openFolderCallback, this, graphicsApplicationWindow, NULL, false);
        }
        static const char* formatNames[] = { "KITTI", "nuScenes" };
        int formatIndex = static_cast<int>(currentFormat);
        ImGui::SetNextItemWidth(120.0f);
        if(ImGui::Combo("##DatasetFormat", &formatIndex, formatNames, IM_ARRAYSIZE(formatNames))) {
            currentFormat = static_cast<DatasetFormat>(formatIndex);
}

        if(!framePaths.empty() && framePaths.size() > 1){
            if(ImGui::Button(ICON_FA_BACKWARD_FAST, iconSize)) {
                frame = 0;
                loadFrame(frame);
            }
            if(ImGui::Button(ICON_FA_BACKWARD_STEP, iconSize)) {
                frame = std::max(0, frame - 1);
                loadFrame(frame);
            }
            if(ImGui::Button(isPlaying ? ICON_FA_PAUSE : ICON_FA_PLAY, iconSize)) {isPlaying = !isPlaying;}
            if(ImGui::Button(ICON_FA_FORWARD_STEP, iconSize)) {
                frame = std::min(frameMax, frame + 1);
                loadFrame(frame);
            }
            if(ImGui::Button(ICON_FA_FORWARD_FAST, iconSize)) {
                frame = frameMax;
                loadFrame(frame);
            }
        }
      
        ImGui::EndMainMenuBar();
    
        glm::vec3 cameraPos = camera.getCameraPosition();
        // ImGui::ShowDemoWindow();
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Frame: %d / %d", frame, frameMax);
        ImGui::Text("Points: %zu", vertices.size() / 6);
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::Text("Camera Position: %.2f, %.2f, %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("Mode: %s", uiMode ? "UI Mode (Tab to Camera Mode)" : "Camera Mode: (Tab to UI Mode)");
        ImGui::End();

        if(fileLoaded) {
                loadFrame(0);
                fileLoaded = false;
            }

        if(framesLoaded) {
            loadFrame(frame);
            framesLoaded = false;
        } else if(isPlaying && !framePaths.empty()) {
            if(frameChangeInterval <= 0) {
                loadFrame(frame);
                frame++;
                frameChangeInterval = 1.0f / formatInfo(currentFormat).frameInterval;

                if(frame > frameMax) {
                    isPlaying = false;
                    frame = 0;
                }
            } else frameChangeInterval -= deltaTime;
        }

        renderer->draw(camera.getViewMatrix(), camera.getProjectionMatrix((float)screenWidth / screenHeight));
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(graphicsApplicationWindow);
    }
}

bool Application::input(float deltaTime) {
    SDL_Event e;

    const bool* keyState = SDL_GetKeyboardState(NULL);
    if(keyState[SDL_SCANCODE_ESCAPE]) {
        std::cout << "Goodbye!" << std::endl;
        return true;
    }

    if(!uiMode) {
            // camera update
        if(keyState[SDL_SCANCODE_W]) camera.updatePosition(Direction::FORWARD, deltaTime);
        if(keyState[SDL_SCANCODE_S]) camera.updatePosition(Direction::BACKWARD, deltaTime);
        if(keyState[SDL_SCANCODE_A]) camera.updatePosition(Direction::LEFT, deltaTime);
        if(keyState[SDL_SCANCODE_D]) camera.updatePosition(Direction::RIGHT, deltaTime);
    }

    while(SDL_PollEvent(&e) != 0) {
        ImGui_ImplSDL3_ProcessEvent(&e);

        if(e.type == SDL_EVENT_QUIT) return true;

        if(e.type == SDL_EVENT_KEY_UP) {
            if(e.key.key == SDLK_TAB) {
                uiMode = !uiMode;
                SDL_SetWindowRelativeMouseMode(graphicsApplicationWindow, !uiMode);
            }

            if(e.key.key == SDLK_SPACE) {
                isPlaying = !isPlaying;
            }

            if(!isPlaying & framePaths.size() > 1) {
                if(e.key.key == SDLK_LEFT) {
                    frame = std::max(0, frame - 1);
                    loadFrame(frame);
                } 
                if(e.key.key == SDLK_RIGHT) {
                    frame = std::min(frameMax, frame + 1);
                    loadFrame(frame);
                }
            }
        }

        if(e.type == SDL_EVENT_MOUSE_MOTION && !uiMode) {
            camera.updateFront(e.motion.xrel, -e.motion.yrel);
        }

        if(e.type == SDL_EVENT_WINDOW_RESIZED) {
            screenWidth = e.window.data1;
            screenHeight = e.window.data2;
            glViewport(0, 0, screenWidth, screenHeight);
        }
    }

    return false;
}

void Application::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    renderer.reset();

    SDL_GL_DestroyContext(openGLContext);
    SDL_DestroyWindow(graphicsApplicationWindow);
    SDL_Quit();
}

glm::vec3 Application::colormap(float t) {
    static const glm::vec3 stops[] = {
        {0.0f, 0.0f, 0.5f},  // dark blue
        {0.0f, 1.0f, 1.0f},  // cyan
        {0.0f, 1.0f, 0.0f},  // green
        {1.0f, 1.0f, 0.0f},  // yellow
        {1.0f, 0.0f, 0.0f},  // red
    };
    constexpr int numStops = sizeof(stops) / sizeof(stops[0]);

    float scaled = t * (numStops - 1);
    int idx = static_cast<int>(scaled);
    if (idx >= numStops - 1) return stops[numStops - 1];

    float frac = scaled - idx;
    return glm::mix(stops[idx], stops[idx + 1], frac);
}

std::vector<float> Application::loadVerticesFromBin(std::string filename, int stride, float groundOffset) {
    std::vector<float> vertices;
    std::vector<float> intensities;
    float x, y, z, intensity;
    size_t numFloats {0};
    std::streamsize fileSize {0};
    std::ifstream binFile(filename, std::ios::binary);

    if(!binFile.is_open()) {
        std::cout << "Error opening file." << std::endl;
        return vertices;
    }

    binFile.seekg(0, std::ios::end);
    fileSize = binFile.tellg();
    binFile.seekg(0, std::ios::beg);

    numFloats = fileSize / sizeof(float);

    std::vector<float> rawData(numFloats);
    binFile.read(reinterpret_cast<char*>(rawData.data()), fileSize);

    for(int i = 3; i < numFloats; i+= stride) {
        intensities.push_back(rawData[i]);
    }

    size_t numIntensities {intensities.size()};
    size_t lowIdx = static_cast<size_t>(0.01 * (numIntensities - 1));
    size_t highIdx = static_cast<size_t>(0.99 * (numIntensities - 1));

    std::nth_element(intensities.begin(), intensities.begin() + lowIdx, intensities.end());
    float p1 = intensities[lowIdx];

    std::nth_element(intensities.begin(), intensities.begin() + highIdx, intensities.end());
    float p99 = intensities[highIdx];
    
    for(int i = 0; i + 3 < numFloats; i += stride) {
        x = rawData[i];
        y = rawData[i + 1];
        z = rawData[i + 2] + groundOffset;
        intensity = rawData[i + 3];

        float t = std::clamp((intensity - p1) / (p99 - p1), 0.0f, 1.0f);
        glm::vec3 color = colormap(t);

        // vertices.push_back(x);
        // vertices.push_back(y);
        // vertices.push_back(z);

        // adjusting coordinates to account for the data coordinate system
        vertices.push_back(-y);     // x value
        vertices.push_back(z);      // y value
        vertices.push_back(-x);     // z value

        // greyscale initially for the color
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    }

    return vertices;
}

void SDLCALL Application::openFolderCallback(void* userData, const char * const *fileList, int filter) {
    if(!fileList || !*fileList) {
        std::cout << "Error | The dialog was closed | No folder selected." << "\n";
        return;
    }

    std::filesystem::path datasetFolder(*fileList);
    Application* app {static_cast<Application*>(userData)};

    if(app->framePaths.size() != 0) app->framePaths.clear();
    app->framesLoaded = false;

    for(auto const& dir_entry : std::filesystem::directory_iterator(datasetFolder)) {
        app->framePaths.push_back(dir_entry.path());
    }
    
    std::sort(app->framePaths.begin(), app->framePaths.end());

    if(app->framePaths.empty()) {
        std::cout << "No files found in the folder.\n";
    }

    app->frameMax = app->framePaths.size() - 1;
    app->frame = 0;
    app->framesLoaded = true;

    std::cout << datasetFolder << "\n";
    std::cout << app->framePaths.size() << "\n";
}

void SDLCALL Application::openFileCallback(void* userData, const char * const *fileList, int filter) {
    if(!fileList || !*fileList) {
        std::cout << "Error | The dialog was closed | No folder selected." << "\n";
        return;
    }

    Application* app {static_cast<Application*>(userData)};

    if(app->framePaths.size() != 0) app->framePaths.clear();

    app->fileLoaded = false;
    app->frame = 0;
    app->frameMax = 0;
    app->isPlaying = false;
    app->framePaths.push_back(std::filesystem::path(*fileList));
    app->fileLoaded = true;
}

void Application::loadFrame(int frame) {
    FormatInfo info = formatInfo(currentFormat);
    vertices = loadVerticesFromBin(framePaths[frame].generic_string(), info.stride, info.groundOffset);
    renderer->setVertices(vertices);
}

FormatInfo Application::formatInfo(DatasetFormat fmt) const {
    switch(fmt) {
        case DatasetFormat::KITTI : return {4, 1.73f, 10.0f};
        case DatasetFormat::NUSCENES : return {5, 1.84f, 20.0f};
    }

    // default
    return {4, 1.73f, 10.0f};
}