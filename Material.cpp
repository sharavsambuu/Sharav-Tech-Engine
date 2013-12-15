#include "Material.hpp"
#include "SOIL.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Material::Material() {
    this->diffuseColor = glm::vec4(0.5, 0.5, 0.5, 1.0); // Medium-gray
    this->ambientColor = glm::vec3(0.5); // Medium-gray 
    this->specularColor = glm::vec3(0.0); // No specular
    this->emissiveColor = glm::vec3(0.0); // No emission
    this->shininess = 0.5;
    this->hasAmbientTexture = false;
    this->hasDiffuseTexture = false;
    this->hasNormalTexture = false;
    this->hasOpacityTexture = false;
    this->hasSpecularTexture = false;
    this->ambientTextureID = 0;
    this->diffuseTextureID = 0;
    this->normalTextureID = 0;
    this->specularTextureID = 0;
    this->opacityTextureID = 0;
    this->isLightVolumeMaterial = false;
}

Material::~Material() {
    glDeleteTextures(1, &this->diffuseTextureID);
    glDeleteTextures(1, &this->normalTextureID);
    glDeleteTextures(1, &this->specularTextureID);
    glDeleteTextures(1, &this->ambientTextureID);
    glDeleteTextures(1, &this->opacityTextureID);
    //std::cout << "<<<<< Material destructor function" << std::endl;
}

void Material::loadDiffuseTexture(const std::string& path) {
    std::string fullPath = "models/" + path;
    this->diffuseTextureID = SOIL_load_OGL_texture(
            fullPath.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_POWER_OF_TWO
            | SOIL_FLAG_MIPMAPS
            | SOIL_FLAG_MULTIPLY_ALPHA
            | SOIL_FLAG_COMPRESS_TO_DXT
            | SOIL_FLAG_DDS_LOAD_DIRECT
            | SOIL_FLAG_INVERT_Y
            );
    if (this->diffuseTextureID == 0) {
        this->hasDiffuseTexture = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::loadNormalTexture(const std::string& path) {
    std::string fullPath = "models/" + path;
    this->normalTextureID = SOIL_load_OGL_texture(
            fullPath.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_POWER_OF_TWO
            | SOIL_FLAG_MIPMAPS
            | SOIL_FLAG_MULTIPLY_ALPHA
            | SOIL_FLAG_COMPRESS_TO_DXT
            | SOIL_FLAG_DDS_LOAD_DIRECT
            | SOIL_FLAG_INVERT_Y
            );
    if (this->normalTextureID == 0) {
        this->hasNormalTexture = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::loadSpecularTexture(const std::string& path) {
    std::string fullPath = "models/" + path;
    this->specularTextureID = SOIL_load_OGL_texture(
            fullPath.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_POWER_OF_TWO
            | SOIL_FLAG_MIPMAPS
            | SOIL_FLAG_MULTIPLY_ALPHA
            | SOIL_FLAG_COMPRESS_TO_DXT
            | SOIL_FLAG_DDS_LOAD_DIRECT
            | SOIL_FLAG_INVERT_Y
            );
    if (this->specularTextureID == 0) {
        this->hasSpecularTexture = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::loadAmbientTexture(const std::string& path) {
    std::string fullPath = "models/" + path;
    this->ambientTextureID = SOIL_load_OGL_texture(
            fullPath.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_POWER_OF_TWO
            | SOIL_FLAG_MIPMAPS
            | SOIL_FLAG_MULTIPLY_ALPHA
            | SOIL_FLAG_COMPRESS_TO_DXT
            | SOIL_FLAG_DDS_LOAD_DIRECT
            | SOIL_FLAG_INVERT_Y
            );
    if (this->ambientTextureID == 0) {
        this->hasAmbientTexture = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::loadOpacityTexture(const std::string& path) {
    std::string fullPath = "models/" + path;
    this->opacityTextureID = SOIL_load_OGL_texture(
            fullPath.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_POWER_OF_TWO
            | SOIL_FLAG_MIPMAPS
            | SOIL_FLAG_MULTIPLY_ALPHA
            | SOIL_FLAG_COMPRESS_TO_DXT
            | SOIL_FLAG_DDS_LOAD_DIRECT
            | SOIL_FLAG_INVERT_Y
            );
    if (this->opacityTextureID == 0) {
        this->hasOpacityTexture = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::bindMaterial(GLuint programID) {
    if (!isLightVolumeMaterial) {
        GLuint diffuseTextureUniformID = glGetUniformLocation(programID, "material_diffuse_texture");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->diffuseTextureID);
        glUniform1i(diffuseTextureUniformID, 0);

        GLuint normalTextureUniformID = glGetUniformLocation(programID, "material_normal_texture");
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->normalTextureID);
        glUniform1i(normalTextureUniformID, 1);

        GLuint specularTextureUniformID = glGetUniformLocation(programID, "material_specular_texture");
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, this->specularTextureID);
        glUniform1i(specularTextureUniformID, 2);

        GLuint ambientTextureUniformID = glGetUniformLocation(programID, "material_ambient_texture");
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, this->ambientTextureID);
        glUniform1i(ambientTextureUniformID, 3);

        GLuint opacityTextureUniformID = glGetUniformLocation(programID, "material_opacity_texture");
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, this->opacityTextureID);
        glUniform1i(opacityTextureUniformID, 4);
    }

    GLuint shininessUniformID = glGetUniformLocation(programID, "material_shininess");
    glUniform1f(shininessUniformID, this->shininess);

    GLuint diffuseColorUniformID = glGetUniformLocation(programID, "material_diffuse_color");
    glUniform3f(diffuseColorUniformID, this->diffuseColor.x, this->diffuseColor.y, this->diffuseColor.z);

    GLuint ambientColorUniformID = glGetUniformLocation(programID, "material_ambient_color");
    glUniform3f(ambientColorUniformID, this->ambientColor.x, this->ambientColor.y, this->ambientColor.z);

    GLuint specularColorUniformID = glGetUniformLocation(programID, "material_specular_color");
    glUniform3f(specularColorUniformID, this->specularColor.x, this->specularColor.y, this->specularColor.z);

    GLuint emissiveColorUniformID = glGetUniformLocation(programID, "material_emissive_color");
    glUniform3f(emissiveColorUniformID, this->emissiveColor.x, this->emissiveColor.y, this->emissiveColor.z);

    if (!isLightVolumeMaterial) {
        GLuint hasDiffuseMapUniformID = glGetUniformLocation(programID, "hasDiffuseMap");
        if (this->hasDiffuseTexture) {
            glUniform1i(hasDiffuseMapUniformID, 1);
        } else {
            glUniform1i(hasDiffuseMapUniformID, 0);
        }
        GLuint hasOpacityMapUniformID = glGetUniformLocation(programID, "hasOpacityMap");
        if (this->hasOpacityTexture) {
            glUniform1i(hasOpacityMapUniformID, 1);
        } else {
            glUniform1i(hasOpacityMapUniformID, 0);
        }
        GLuint hasAmbientMapUniformID = glGetUniformLocation(programID, "hasAmbientMap");
        if (this->hasAmbientTexture) {
            glUniform1i(hasAmbientMapUniformID, 1);
        } else {
            glUniform1i(hasAmbientMapUniformID, 0);
        }
        GLuint hasNormalMapUniformID = glGetUniformLocation(programID, "hasNormalMap");
        if (this->hasNormalTexture) {
            glUniform1i(hasNormalMapUniformID, 1);
        } else {
            glUniform1i(hasNormalMapUniformID, 0);
        }
        GLuint hasSpecularMapUniformID = glGetUniformLocation(programID, "hasSpecularMap");
        if (this->hasSpecularTexture) {
            glUniform1i(hasSpecularMapUniformID, 1);
        } else {
            glUniform1i(hasSpecularMapUniformID, 0);
        }
    }
}

void Material::unbindMaterial() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}