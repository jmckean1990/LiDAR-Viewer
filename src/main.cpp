#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <random>
#include <fstream>
#include <string>


int gScreenWidth = 680;
int gScreenHeight = 480;
unsigned int VBO;
unsigned int VAO;
unsigned int shaderProgram;
SDL_Window*     gGraphicsApplicationWindow = nullptr;
SDL_GLContext   gOpenGLContext = nullptr;

glm::vec3 cameraPos {glm::vec3(0.0f, 0.0f, 3.0f)};
glm::vec3 cameraFront {glm::vec3(0.0f, 0.0f, -1.0)};
glm::vec3 cameraUp {glm::vec3(0.0f, 1.0f, 0.0f)};

Uint64 ticks {0};
float timeSeconds{0.0f};
float deltaTime {0.0f};
float lastFrame {0.0f};
    

const char* vertexShaderSource = "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 vColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   vColor = aColor;\n"
    "}\0";

const char* fragmentShaderSource = "#version 410 core\n"
    "in vec3 vColor;"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   vec2 coord = gl_PointCoord - vec2(0.5f);"
    "   if(length(coord) > 0.5) discard;"
    "   FragColor = vec4(vColor, 1.0f);\n"
    "}\0";

bool gQuit = false;

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

unsigned int shaderSetupRun() {
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

    return shaderProgram;
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
    float cameraSpeed {0.0f};
    ticks = SDL_GetTicks();
    timeSeconds = ticks / 1000.0f;
    deltaTime = timeSeconds - lastFrame;
    lastFrame = timeSeconds;
    cameraSpeed = 2.5f * deltaTime;

    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_EVENT_QUIT) {
            std::cout << "Goodbye!" << std::endl;
            gQuit = true;
        }
    }

        const bool* keyState = SDL_GetKeyboardState(NULL);
        if(keyState[SDL_SCANCODE_W]) cameraPos += cameraSpeed * cameraFront;
        if(keyState[SDL_SCANCODE_S]) cameraPos -= cameraSpeed * cameraFront;
        if(keyState[SDL_SCANCODE_A]) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if(keyState[SDL_SCANCODE_D]) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

void PreDraw() {

}

std::vector<float> generateVertices(int numVertices) {
    std::vector<float> vertices;
    // allocate space for 3 position values and 3 color values per vertex
    vertices.reserve(3 * 2 * numVertices);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);


    for(int i = 0; i < numVertices; ++i) {
        // add x, y, z position
        vertices.push_back(posDist(gen));
        vertices.push_back(posDist(gen));
        vertices.push_back(posDist(gen));

        // add r, g, b pixel color
        vertices.push_back(colorDist(gen));
        vertices.push_back(colorDist(gen));
        vertices.push_back(colorDist(gen));
    }

    return vertices;
}

std::vector<float> loadVerticesFromFile(std::string filename) {
    std::vector<float> vertices;
    float x, y, z;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
    std::string line;
    std::ifstream bunnyFile(filename);

    if(!bunnyFile.is_open()) {
        std::cout << "Error Opening File" << std::endl;
        return vertices;
    }

    // ignore header
    while(line != "end_header") {
        std::getline(bunnyFile, line);
        std::cout << line << std::endl;
    }

    for(int i = 0; i < 30'000; ++i) {
        bunnyFile >> x >> y >> z;
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        vertices.push_back(colorDist(gen));
        vertices.push_back(colorDist(gen));
        vertices.push_back(colorDist(gen));
    }


    return vertices;
}

int numVertices = 10'000;
// std::vector<float> vertices = generateVertices(numVertices);
std::vector<float> vertices = loadVerticesFromFile("bun090.ply");

void setupGraphics() {
    glPointSize(5.0f);
    glEnable(GL_DEPTH_TEST);
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
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Link Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shaderProgram = shaderSetupRun();

    // activate the program
    glUseProgram(shaderProgram);
}

void Draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // glm::mat4 tMat = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, 0.5f));
    // Uint64 ticks = SDL_GetTicks();
    // float timeSeconds = ticks / 1000.0f;
    // tMat = glm::rotate(tMat, timeSeconds, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // model = glm::rotate(model, timeSeconds, glm::vec3(1.0f, 0.0f, 0.0f));
    
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(
        cameraPos,
        cameraPos + cameraFront,
        cameraUp
    );

    // view = glm::rotate(view, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // view = glm::translate(view, glm::vec3(0.0f, -2.0f, -5.0f));

    // glm::vec3 cameraPos {glm::vec3(0.0f, 0.0f, 3.0f)};    
    // glm::vec3 cameraTarget {glm::vec3(0.0f, 0.0f, 0.0f)};
    // glm::vec3 cameraDirection {glm::normalize(cameraPos - cameraTarget)};
    // glm::vec3 up {glm::vec3(0.0f, 1.0f, 0.0f)};
    // glm::vec3 cameraRight {glm::normalize(glm::cross(up, cameraDirection))};
    // glm::vec3 cameraUp {glm::normalize(glm::cross(cameraDirection, cameraRight))};

    // view = glm::lookAt(
    //     cameraPos,
    //     cameraTarget,
    //     up
    // );

    // rotate around 0, 0, 0
    // const float radius = 10.0f;
    // float camX = sin(timeSeconds) * radius;
    // float camZ = cos(timeSeconds) * radius;
    // view = glm::lookAt(
    //     glm::vec3(camX, 0.0f, camZ),
    //     cameraTarget,
    //     up
    // );

    


    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)gScreenWidth / gScreenHeight, 0.1f, 100.0f);


    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    modelLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(view));

    modelLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, numVertices);
}


void MainLoop() {
    setupGraphics();

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