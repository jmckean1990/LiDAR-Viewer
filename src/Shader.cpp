#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>


Shader::Shader(std::string vShaderFile, std::string fShaderFile) {
    std::ifstream vertexFile(vShaderFile);
    std::ifstream fragmentFile(fShaderFile);
    std::string vertexContentStr, fragmentContentStr;

    if(!vertexFile.is_open() || !fragmentFile) {
        std::cout << "Error opening the vertex and/or fragment shader file." << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << vertexFile.rdbuf();
    vertexContentStr = buffer.str();
    
    buffer.clear();
    buffer.str(std::string());

    buffer << fragmentFile.rdbuf();
    fragmentContentStr = buffer.str();

    GLuint vertexId {Shader::compile(GL_VERTEX_SHADER, vertexContentStr)};
    GLuint fragmentId {Shader::compile(GL_FRAGMENT_SHADER, fragmentContentStr)};
    
    if(!vertexId || !fragmentId) {
        glDeleteShader(vertexId);
        glDeleteShader(fragmentId);
        return;
    }

    GLuint programId {glCreateProgram()};
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);
    glLinkProgram(programId);

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);

    GLint success {0};
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    
    if(!success) {
        GLsizei logLength {0};
        GLsizei written {0};
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(logLength, '\0');
        glGetProgramInfoLog(programId, logLength, &written, log.data());
        log.resize(written);
        std::cout << "Error linking." << "\n"
                << log << "\n";

        glDeleteProgram(programId);
        return;
    }
    
    id = programId;
}

Shader::~Shader() {
    glDeleteProgram(id);
}


GLuint Shader::compile(GLenum type, std::string& source) {
    GLuint shaderId {0};
    const char* src = source.c_str();
 
    shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &src, NULL);
    glCompileShader(shaderId);

    GLint success {0};
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLsizei logLength {0};
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    
        std::string log(logLength, '\0');
        GLsizei written {0};

        glGetShaderInfoLog(shaderId, logLength, &written, log.data());
        log.resize(written);    // gets rid of the trailing \0
        std::cout << "Error Compiling the shader." << "\n"
                << log << "\n";
        
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

void Shader::use() const {
    if(!isValid()) {
        std::cout << "Invalid program id" << "\n";
        return;
    }

    glUseProgram(id);
}

void Shader::setMat4Uniform(const std::string& name, const glm::mat4& data) const {
    GLint modelLoc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(data));
}
