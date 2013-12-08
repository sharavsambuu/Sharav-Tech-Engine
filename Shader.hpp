/* 
 * File:   Shader.hpp
 * Author: prompto
 *
 * Created on December 1, 2013, 6:03 PM
 */

#ifndef SHADER_HPP
#define	SHADER_HPP

#include <iostream>
#include "GL/glew.h"

enum SHADERTYPE {
    VERTEX, FRAGMENT, GEOMETRY, TESS_EVAL, TESS_CNTRL
};

class Shader {
public:
    Shader(std::string name, SHADERTYPE type);
    ~Shader();

    void Compile();
    void Load(std::string name);

    inline GLuint GetShader() const {
        return shaderID;
    }

private:
    std::string name;
    std::string *source;
    bool isCompiled;
    SHADERTYPE shaderType;

    GLuint shaderID;
};


#endif	/* SHADER_HPP */

