/* 
 * File:   ShaderProgram.hpp
 * Author: prompto
 *
 * Created on December 1, 2013, 6:10 PM
 */

#ifndef SHADERPROGRAM_HPP
#define	SHADERPROGRAM_HPP

#define GLEW_STATIC
#include "GL/glew.h"
#include <iostream>
#include <vector>
#include <map>
#include "Shader.hpp"

class ShaderProgram {
public:
    ShaderProgram(std::string name);
    ~ShaderProgram();

    void Activate();
    void Deactivate();

    void AttachShader(Shader *shader);
    void BindAttribute(GLuint index, std::string attrName);
    void Link();

    inline GLuint GetID() const {
        return programID;
    }

private:
    bool isLinked;
    std::vector<Shader *> shaders;
    GLuint programID;
    std::string programName;
    std::map<std::string, GLuint> attribs;
    bool isActive;
};

#endif	/* SHADERPROGRAM_HPP */

