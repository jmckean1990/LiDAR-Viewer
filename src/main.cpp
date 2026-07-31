#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <iostream>


// Globals
int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window*     gGraphicsApplicationWindow = nullptr;
SDL_GLContext   gOpenGLContext = nullptr;

const char* vertexShaderSource = "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 410 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

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
    float vertices[] {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    unsigned int VBO;
    unsigned int VAO;
    // generates one or more buffer objects
    // returns a object id
    // params: number of buffers, address to store the ID
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    // bind the VAO
    glBindVertexArray(VAO);

    // binds newly created buffer to the GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // allocates memory and stores data in the initialized memory of the bound buffer object
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Virtex Shader
    // create a shader object
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // attach the shader source code to the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // compile the shader
    glCompileShader(vertexShader);

    // check if shader copilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED]N" << infoLog << std::endl;
    }
    
    // link shader objects into a shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // attach shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    // link
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }

    // delete shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(vertexShader);

    // Link Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // activate the program
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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