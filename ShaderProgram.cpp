#include "ShaderProgram.hpp"
#include "stdlib.h"

ShaderProgram::ShaderProgram(std::string name) {
    programID = glCreateProgram();
    std::cerr << "created program ID is " << programID << "\n";
    this->programName = name;
    isLinked = false;
    isActive = false;
}

ShaderProgram::~ShaderProgram() {
    std::cerr << "deleting shader program " << programName << "\n";
    glDeleteProgram(programID);
}

void ShaderProgram::Activate() {
    glUseProgram(programID);
    isActive = true;
}

void ShaderProgram::Deactivate() {
    isActive = false;
    glUseProgram(0);
}

void ShaderProgram::AttachShader(Shader *shader) {
    shaders.push_back(shader);
    glAttachShader(programID, shader->GetShader());
}

void ShaderProgram::BindAttribute(GLuint index, std::string attrName) {
    if (isLinked == true) {
        std::cerr << "warning binding attribute after link\n";
    }
    attribs[attrName] = index;
    glBindAttribLocation(programID, index, attrName.c_str());
    std::cerr << "bind attribute location " << programID << " index " << index
            << " name " << attrName << "\n";
}

void ShaderProgram::Link() {
    glLinkProgram(programID);
    GLint infologLength = 0;
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
        char *infoLog = new char[infologLength];
        GLint charsWritten = 0;
        glGetProgramInfoLog(programID, infologLength, &charsWritten, infoLog);
        std::cerr << infoLog << std::endl;
        delete[] infoLog;
        glGetProgramiv(programID, GL_LINK_STATUS, &infologLength);
        if (infologLength == GL_FALSE) {
            std::cerr << "program linking failed and exiting\n";
            exit(EXIT_FAILURE);
        }
    }
    isLinked = true;
}

