/* 
 * File:   ShaderManager.hpp
 * Author: prompto
 *
 * Created on December 1, 2013, 6:13 PM
 */

#ifndef SHADERMANAGER_HPP
#define	SHADERMANAGER_HPP

#include "GL/glew.h"
#include <iostream>
#include <vector>
#include <map>
#include "Shader.hpp"
#include "ShaderProgram.hpp"

class ShaderManager {
public:
    static ShaderManager* getSingleton();
    ShaderManager();
    ~ShaderManager();

    bool LinkShader(std::string name);
    void CreateShaderProgram(std::string name);
    void AttachShader(std::string name, SHADERTYPE type);
    void AttachShaderToProgram(std::string program, std::string shader);
    GLuint GetProgramID(std::string name);
    void CompileShader(std::string name);
    void LinkProgramObject(std::string name);
    void Use(std::string name);
    void BindAttribute(std::string programName, GLuint index, std::string attribName);
    void LoadShaderSource(std::string shaderName, std::string sourceFile);
    ShaderProgram * operator[](const std::string &programName);
    bool isUsingGBuffer;
private:
    std::map<std::string, ShaderProgram *> shaderPrograms;
    std::map<std::string, Shader *> shaders;
    
    static ShaderManager* instance;
};

#endif	/* SHADERMANAGER_HPP */

