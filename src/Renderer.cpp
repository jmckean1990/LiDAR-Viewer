#include "Renderer.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>


#include <iostream>


Renderer::Renderer(const std::string vertexShaderFile, const std::string fragmentShaderFile)
        : shader(vertexShaderFile, fragmentShaderFile) {
    
    glPointSize(5.0f);
    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Renderer::draw(const glm::mat4& view, const glm::mat4& projection) const {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);

    shader.use();
    shader.setMat4Uniform("model", model);
    shader.setMat4Uniform("view", view);
    shader.setMat4Uniform("projection", projection);

    // glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, pointCount);
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