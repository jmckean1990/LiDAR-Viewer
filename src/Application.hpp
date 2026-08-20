#pragma once

#include "FlyCamera.hpp"
#include "Renderer.hpp"
#include "Dataset.hpp"

#include <SDL3/SDL.h>

#include <optional>
#include <filesystem>


class Application {
public:
    std::vector<float> vertices;  

    int frame = 0, frameMax = 0;

    Application();
    ~Application() {shutdown();}    

    void run();
    
private:
    int screenWidth = 1280;
    int screenHeight = 720;
    bool uiMode = true;
    bool running = false;
    bool framesLoaded = false;
    bool fileLoaded = false;
    bool isPlaying = false;
  
    SDL_Window*     graphicsApplicationWindow = nullptr;
    SDL_GLContext   openGLContext = nullptr;
    
    FlyCamera camera;
    std::optional <Renderer> renderer;
    Dataset dataset;

    bool init();
    bool input(float deltaTime);
    void shutdown();
    void loadFrame(int frame);
    glm::vec3 colormap(float t);
    std::vector<float> loadVerticesFromBin(std::string filename, int stride, float groundOffset);
    static void SDLCALL openFolderCallback(void* userData, const char * const *fileList, int filter);
    static void SDLCALL openFileCallback(void* userData, const char * const *fileList, int filter);
};

