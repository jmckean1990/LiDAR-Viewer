#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <algorithm>

#include "Application.hpp"


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
    float x, y, z, confidence, intensity;
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
        // std::cout << line << std::endl;
    }

    for(int i = 0; i < 35'947; ++i) {
        bunnyFile >> x >> y >> z >> confidence >> intensity;
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        vertices.push_back(colorDist(gen));
        vertices.push_back(colorDist(gen));
        vertices.push_back(colorDist(gen));
    }

    return vertices;
}


int main() {
    Application app;
    
    app.run();
}
