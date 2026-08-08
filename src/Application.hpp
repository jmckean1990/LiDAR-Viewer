#pragma once

#include "FlyCamera.hpp"
#include "Renderer.hpp"

#include <SDL3/SDL.h>

#include <optional>

class Application {
public:
    Application();
    ~Application() {shutdown();}    

    void run();
    

private:
    int screenWidth = 1280;
    int screenHeight = 720;
    int frame = 0, frameMax = 153; 
    SDL_Window*     graphicsApplicationWindow = nullptr;
    SDL_GLContext   openGLContext = nullptr;
    Uint64 ticks {0};
    float timeSeconds{0.0f};
    float deltaTime {0.0f};
    float lastFrame {0.0f};
    bool uiMode = false;
    
    std::vector<float> vertices;
    
    FlyCamera camera;
    std::optional <Renderer> renderer;

    std::string frameFile = "000000000";
    std::string datasetFolder {"datasets/KITTI"};

    bool running = false;
    
    bool init();
    bool input();
    void shutdown();
    glm::vec3 colormap(float t);
    std::vector<float> loadVerticesFromBin(std::string filename);
};

