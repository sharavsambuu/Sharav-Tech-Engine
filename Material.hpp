/* 
 * File:   Material.hpp
 * Author: prompto
 *
 * Created on November 30, 2013, 7:45 PM
 */

#ifndef MATERIAL_HPP
#define	MATERIAL_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Material {
public:
    Material();
    ~Material();

    glm::vec4 getDiffuseColor() {
        return this->diffuseColor;
    };

    void setDiffuseColor(glm::vec4 color) {
        this->diffuseColor = color;
    }

    glm::vec3 getAmbientColor() {
        return this->ambientColor;
    }

    void setAmbientColor(glm::vec3 color) {
        this->ambientColor = color;
    }

    glm::vec3 getSpecularColor() {
        return this->specularColor;
    }

    void setSpecularColor(glm::vec3 color) {
        this->specularColor = color;
    }

    glm::vec3 getEmissiveColor() {
        return this->emissiveColor;
    }

    void setEmissiveColor(glm::vec3 color) {
        this->emissiveColor = color;
    }

    float getShininess() {
        return this->shininess;
    }

    void setShininess(float shininess) {
        this->shininess = shininess;
    }

    GLuint getDiffuseTextureID() {
        return this->diffuseTextureID;
    }

    void setDiffuseTextureID(GLuint diffuseTextureID) {
        this->diffuseTextureID = diffuseTextureID;
    }

    GLuint getNormalTextureID() {
        return this->normalTextureID;
    }

    void setNormalTextureID(GLuint normalTextureID) {
        this->normalTextureID = normalTextureID;
    }

    GLuint getSpecularTextureID() {
        return this->specularTextureID;
    }

    void setSpecularTextureID(GLuint specularTextureID) {
        this->specularTextureID = specularTextureID;
    }

    GLuint getAmbientTextureID() {
        return this->ambientTextureID;
    }

    void setAmbientTextureID(GLuint ambientTextureID) {
        this->ambientTextureID = ambientTextureID;
    }

    GLuint getOpacityTextureID() {
        return this->opacityTextureID;
    }

    void setOpacityTextureID(GLuint opacityTextureID) {
        this->opacityTextureID = opacityTextureID;
    }

    void loadDiffuseTexture(const std::string &path);
    void loadNormalTexture(const std::string &path);
    void loadSpecularTexture(const std::string &path);
    void loadAmbientTexture(const std::string &path);
    void loadOpacityTexture(const std::string &path);

    void bindMaterial(GLuint programID);
    void unbindMaterial();

    void setLightMaterialBool(bool b) {
        this->isLightVolumeMaterial = b;
    }

    bool hasDiffuseTexture;
    bool hasNormalTexture;
    bool hasSpecularTexture;
    bool hasAmbientTexture;
    bool hasOpacityTexture;

private:
    bool isLightVolumeMaterial;
    glm::vec4 diffuseColor;
    glm::vec3 ambientColor;
    glm::vec3 specularColor;
    glm::vec3 emissiveColor;
    float shininess;
    GLuint diffuseTextureID;
    GLuint normalTextureID;
    GLuint specularTextureID;
    GLuint ambientTextureID;
    GLuint opacityTextureID;

};

#endif	/* MATERIAL_HPP */

