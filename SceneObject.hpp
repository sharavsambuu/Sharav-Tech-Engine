/* 
 * File:   SceneObject.hpp
 * Author: prompto
 *
 * Created on December 1, 2013, 6:51 PM
 */

#ifndef SCENEOBJECT_HPP
#define	SCENEOBJECT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "MaterialManager.hpp"

typedef struct {
    GLuint vertexID;
    GLuint normalID;
    GLuint texcoordID;
    GLuint tangentID;
    GLuint bitangentID;
    GLuint vaoID;
    int numFaces;
    int *faces;
    int material;
    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;
} Mesh;

class SceneObject {
public:
    SceneObject();
    virtual ~SceneObject();
    void render(GLuint programID);
    void update(float dt);
    void addMesh(
            float *vertices, int numVertices,
            float *normals, int numNormals,
            float *textureCoordinates, int numTextureCoordinates,
            float *tangents, int numTangents,
            float *bitangents, int numBitangents,
            int *faces, int numFaces,
            int materialIndex);
    void addMesh(
            float *vertices, int numVertices,
            float *normals, int numNormals,
            float *textureCoordinates, int numTextureCoordinates,
            float *tangents, int numTangents,
            float *bitangents, int numBitangents,
            int *faces, int numFaces,
            const std::string &materialName);
    glm::vec3 getBoundingBoxMin();
    glm::vec3 getBoundingBoxMax();

    glm::mat4 getModelMatrix();

    MaterialManager *mm;
protected:
    std::vector<Mesh> meshes;
    glm::mat4 modelMatrix;
    glm::mat4 parentMatrix;
    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;
};

#endif	/* SCENEOBJECT_HPP */

