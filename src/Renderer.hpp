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
    std::vector<float> buildGridVertices(float extent, float step, glm::vec3 color);
    std::vector<float> buildAxisVertices(float length);

private:
    GLuint VBO {0}, VAO {0};
    GLuint gridVBO {0}, gridVAO {0};
    size_t pointCount {0};
    size_t gridVertexCount {0};

    Shader shader;
};