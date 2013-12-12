#include "ShaderManager.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include <fstream>

ShaderManager* ShaderManager::instance = NULL;

ShaderManager* ShaderManager::getSingleton() {
    if (!ShaderManager::instance)
        ShaderManager::instance = new ShaderManager();
    return ShaderManager::instance;
}

ShaderManager::ShaderManager() {
    std::cout << "<<<<< shader manager construction function " << std::endl;
}

ShaderManager::~ShaderManager() {
    std::cout << "<<<<< shader manager desctruction function " << std::endl;
    for (auto& program : shaderPrograms) {
        delete program.second;
    }
    shaderPrograms.clear();

    for (auto& shader : shaders) {
        delete shader.second;
    }
    shaders.clear();
}

void ShaderManager::AttachShader(std::string name, SHADERTYPE type) {
    shaders[name] = new Shader(name, type);
}

void ShaderManager::CompileShader(std::string name) {
    std::map<std::string, Shader *>::const_iterator shader = shaders.find(name);
    if (shader != shaders.end()) {
        shader->second->Compile();
    } else {
        std::cerr << "Warning shader not know in compile " << name.c_str();
    }
}

void ShaderManager::CreateShaderProgram(std::string name) {
    std::cerr << "creating empty ShaderProgram " << name.c_str() << "\n";
    shaderPrograms[name] = new ShaderProgram(name);
}

void ShaderManager::AttachShaderToProgram(std::string _program, std::string _shader) {
    std::map<std::string, Shader *>::const_iterator shader = shaders.find(_shader);
    std::map<std::string, ShaderProgram *>::const_iterator program = shaderPrograms.find(_program);
    if (shader != shaders.end() && program != shaderPrograms.end()) {
        program->second->AttachShader(shader->second);
        std::cerr << _shader.c_str() << " attached to program " << _program.c_str() << "\n";
    } else {
        std::cerr << "Warning cant attach " << _shader.c_str() << " to " << _program.c_str() << "\n";
    }
}

void ShaderManager::LoadShaderSource(std::string _shaderName, std::string _sourceFile) {
    std::map<std::string, Shader *>::const_iterator shader = shaders.find(_shaderName);
    if (shader != shaders.end()) {
        shader->second->Load(_sourceFile);
    } else {
        std::cerr << "Warning shader not know in loadShaderSource " << _shaderName.c_str();
    }
}

void ShaderManager::LinkProgramObject(std::string _name) {
    std::map<std::string, ShaderProgram *>::const_iterator program = shaderPrograms.find(_name);
    if (program != shaderPrograms.end()) {
        std::cerr << "Linking " << _name.c_str() << "\n";
        program->second->Link();
    } else {
        std::cerr << "Warning Program not known in link " << _name.c_str();
    }
}

void ShaderManager::Use(std::string _name) {
    std::map<std::string, ShaderProgram *>::const_iterator program = shaderPrograms.find(_name);
    if (program != shaderPrograms.end()) {
        std::cerr << "Shader manager Use\n";
        program->second->Activate();
    } else {
        std::cerr << "Warning Program not know in use " << _name.c_str();
        glUseProgram(0);
    }
}

GLuint ShaderManager::GetProgramID(std::string _name) {
    std::map<std::string, ShaderProgram *>::const_iterator program = shaderPrograms.find(_name);
    if (program != shaderPrograms.end()) {
        return program->second->GetID();
    } else {
        std::cerr << "Warning Program not know in use " << _name.c_str();
        return -1;
    }
}

void ShaderManager::BindAttribute(std::string _programName, GLuint _index, std::string _attribName) {
    std::map<std::string, ShaderProgram *>::const_iterator program = shaderPrograms.find(_programName);
    if (program != shaderPrograms.end()) {
        program->second->BindAttribute(_index, _attribName);
    } else {
        std::cerr << "Warning Program not know in bindAttrib " << _programName.c_str();
    }
}

void ceckGLError(const std::string &_file, const int _line) {
    /*int err_num = glGetError();
    while (err_num != GL_NO_ERROR) {
            const char* str = (const char*) gluErrorString(err_num);
            err_num = glGetError();
    }*/
}

ShaderProgram * ShaderManager::operator[](const std::string &_programName) {
    std::map<std::string, ShaderProgram *>::const_iterator program = shaderPrograms.find(_programName);
    if (program != shaderPrograms.end()) {
        return program->second;
    } else {
        std::cerr << "Warning Program not know in [] " << _programName.c_str();
        return 0;
    }
}
