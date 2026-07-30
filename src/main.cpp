#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <iostream>


// Globals
int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window*     gGraphicsApplicationWindow = nullptr;
SDL_GLContext   gOpenGLContext = nullptr;

bool gQuit = false;

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void InitializeProgram() {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL3 Could not initialize video subsystem" << std::endl;
        exit(1);
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

    gGraphicsApplicationWindow = SDL_CreateWindow(
                                "OpenGL Window", 
                                gScreenWidth, gScreenHeight,
                                SDL_WINDOW_OPENGL);
    if(gGraphicsApplicationWindow == nullptr) {
        std::cout << "SDL Window could not be created." << std::endl;
        exit(1);
    }

    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);

    if(gOpenGLContext == nullptr) {
        std::cout << "SDL Context not available." << std::endl;
        exit(1);
    }

    // initialize the Glad Library
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "Glad was not initialized." << std::endl;
        exit(1);
    }

    GetOpenGLVersionInfo();
}

void Input() {
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_EVENT_QUIT) {
            std::cout << "Goodbye!" << std::endl;
            gQuit = true;
        }
    }
}

void PreDraw() {

}

void Draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void MainLoop() {
    while(!gQuit) {
        Input();

        PreDraw();

        Draw();

        // Update the screen
        SDL_GL_SwapWindow(gGraphicsApplicationWindow);
    }
}

void CleanUp() {
    SDL_GL_DestroyContext(gOpenGLContext);
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}

int main() {
    InitializeProgram();

    MainLoop();

    CleanUp();

    return 0;
}






// int main()
// {
//     if (!SDL_Init(SDL_INIT_VIDEO)) {
//         std::cerr << "SDL_Init failed: "
//                   << SDL_GetError()
//                   << '\n';
//         return 1;
//     }

//     SDL_Window* window = SDL_CreateWindow(
//         "SDL3 Test",
//         1280,
//         720,
//         SDL_WINDOW_RESIZABLE
//     );

//     if (window == nullptr) {
//         std::cerr << "SDL_CreateWindow failed: "
//                   << SDL_GetError()
//                   << '\n';

//         SDL_Quit();
//         return 1;
//     }

//     bool running = true;

//     while (running) {
//         SDL_Event event{};

//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_EVENT_QUIT) {
//                 running = false;
//             }
//         }

//         SDL_Delay(1);
//     }

//     SDL_DestroyWindow(window);
//     SDL_Quit();

//     return 0;
// }