#pragma once

#include "Shader.hpp"

#include <glm/glm.hpp>

class Renderer {
public:
    Renderer(const std::string vertexShaderFile, const std::string fragmentShaderFile);
    ~Renderer();

    void displayOpenGLVersionInfo();
    void setVertices(const std::vector<float>& vertices);
    void draw(const glm::mat4& view, const glm::mat4& projection) const;

private:
    GLuint VBO {0}, VAO {0};
    size_t pointCount {0};
    Shader shader;
};