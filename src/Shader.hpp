#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
 

class Shader {
public:
    Shader(std::string vShaderFile, std::string fShaderFile);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept : id(other.id) {
        other.id = 0;
    }

    Shader& operator=(Shader&& other) noexcept {
        if(this != &other) {
            glDeleteProgram(id);
            id = other.id;
            other.id = 0;
        }

        return *this;
    }

    bool isValid() const { return id != 0; }
    void use() const;
    void setMat4Uniform(const std::string& name, const glm::mat4& data) const;
    void setBoolUniform(const std::string& name, bool isPoint) const;
    

private:
    GLuint id {0};

    GLuint compile(GLenum type, std::string& shaderSource);


};