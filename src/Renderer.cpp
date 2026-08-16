#include "Renderer.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>


Renderer::Renderer(const std::string vertexShaderFile, const std::string fragmentShaderFile)
        : shader(vertexShaderFile, fragmentShaderFile) {
    
    glPointSize(5.0f);
    glEnable(GL_DEPTH_TEST);

    // Points
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Grid
    glm::vec3 color(0.75f, 0.75f, 0.75f);
    std::vector<float> gridVertices = buildGridVertices(100.0f, 5.0f, color);
    std::vector<float> axisVertices = buildAxisVertices(3.0f);
    gridVertices.insert(gridVertices.end(), axisVertices.begin(), axisVertices.end());
    gridVertexCount = gridVertices.size() / 6;

    glGenBuffers(1, &gridVBO);
    glGenVertexArrays(1, &gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);

    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(gridVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &gridVAO);
}

void Renderer::draw(const glm::mat4& view, const glm::mat4& projection) const {
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClearColor(0.05f, 0.05f, 0.06f, 1.0f);
    glClearColor(0.1f, 0.1f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);

    shader.use();
    shader.setMat4Uniform("model", model);
    shader.setMat4Uniform("view", view);
    shader.setMat4Uniform("projection", projection);

    shader.setBoolUniform("isPoint", true);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, pointCount);

    shader.setBoolUniform("isPoint", false);
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertexCount);
}

void Renderer::displayOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void Renderer::setVertices(const std::vector<float>& vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    pointCount = vertices.size() / 6;
}

std::vector<float> Renderer::buildGridVertices(float extent, float step, glm::vec3 color) {
    std::vector<float> grid;

    // z grid lines, sliding across x-axis
    for(float x = -extent; x <= extent; x += step) {
        if(x == 0.0f) continue;
        grid.insert(grid.end(), {x, 0.0f, -extent, color.r, color.g, color.b});
        grid.insert(grid.end(), {x, 0.0f, extent, color.r, color.g, color.b});
    }

    // x grid lines, sliding across z-axis
    for(float z = -extent; z <= extent; z += step) {
        if(z == 0.0f) continue;
        grid.insert(grid.end(), {-extent, 0.0f, z, color.r, color.g, color.b});
        grid.insert(grid.end(), {extent, 0.0f, z, color.r, color.g, color.b});
    }

    return grid;
}

std::vector<float> Renderer::buildAxisVertices(float length) {
    std::vector<float> axis;
    float liftOffAxis = 0.02f;      // add slight hight to get rid of z fighting

    // x-axis, red
    axis.insert(axis.end(), {-length, 0.0f, liftOffAxis, 1.0f, 0.0f, 0.0f});
    axis.insert(axis.end(), {length, 0.0f, liftOffAxis, 1.0f, 0.0f, 0.0f});

    // y-axis, green
    axis.insert(axis.end(), {0.0f, -length, liftOffAxis, 0.0f, 1.0f, 0.0f});
    axis.insert(axis.end(), {0.0f, length, liftOffAxis, 0.0f, 1.0f, 0.0f});

    // z-axis, blue
    axis.insert(axis.end(), {0.0f, liftOffAxis, -length, 0.0f, 0.0f, 1.0f});
    axis.insert(axis.end(), {0.0f, liftOffAxis, length, 0.0f, 0.0f, 1.0f});

    return axis;
}