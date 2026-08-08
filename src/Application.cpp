#include "Application.hpp"
#include "FlyCamera.hpp"
#include "Renderer.hpp"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <random>
#include <fstream>
#include <algorithm>
#include <vector>


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

    SDL_SetWindowRelativeMouseMode(graphicsApplicationWindow, true);

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

    ImGui_ImplSDL3_InitForOpenGL(graphicsApplicationWindow, openGLContext);
    ImGui_ImplOpenGL3_Init("#version 410");

    return true;
}

void Application::run() {

    vertices = loadVerticesFromBin(datasetFolder + "/" + "0000000000.bin");
    renderer->setVertices(vertices);

    while(!running) {
        running = input();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if(uiMode) {
        glm::vec3 cameraPos = camera.getCameraPosition();
        // ImGui::ShowDemoWindow();
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Frame: %d / %d", frame, frameMax);
        ImGui::Text("Points: %zu", vertices.size() / 6);
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::Text("Camera Position: %.2f, %.2f, %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("Mode: %s", uiMode ? "UI Mode (Tab to Camera Mode)" : "Camera Mode: (Tab to UI Mode)");

        ImGui::End();
    } else {
        ImGui::Begin("Info");
        ImGui::Text("Mode: %s", uiMode ? "UI Mode (Tab to Camera Mode)" : "Camera Mode: (Tab to UI Mode)");
        ImGui::End();
    }

        renderer->draw(camera.getViewMatrix(), camera.getProjectionMatrix((float)screenWidth / screenHeight));
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(graphicsApplicationWindow);
    }
}

bool Application::input() {
    SDL_Event e;

    ticks = SDL_GetTicks();
    timeSeconds = ticks / 1000.0f;
    deltaTime = timeSeconds - lastFrame;
    lastFrame = timeSeconds;
    

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
                std::string frameStr = std::to_string(frame);
                std::string fileStr = frameStr.insert(0, 10 - frameStr.size(), '0') + ".bin";
                std::cout << fileStr << std::endl;
                vertices = loadVerticesFromBin(datasetFolder + "/" + fileStr);
                frame++;

                if(frame > frameMax) frame = 0;

                renderer->setVertices(vertices);
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

std::vector<float> Application::loadVerticesFromBin(std::string filename) {
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

    for(int i = 3; i < numFloats; i+= 4) {
        intensities.push_back(rawData[i]);
    }

    size_t numIntensities {intensities.size()};
    size_t lowIdx = static_cast<size_t>(0.01 * (numIntensities - 1));
    size_t highIdx = static_cast<size_t>(0.99 * (numIntensities - 1));

    std::nth_element(intensities.begin(), intensities.begin() + lowIdx, intensities.end());
    float p1 = intensities[lowIdx];

    std::nth_element(intensities.begin(), intensities.begin() + highIdx, intensities.end());
    float p99 = intensities[highIdx];
    
    for(int i = 0; i + 3 < numFloats; i += 4) {
        x = rawData[i];
        y = rawData[i + 1];
        z = rawData[i + 2];
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