#include "SceneObject.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "MaterialManager.hpp"
#include "ShaderManager.hpp"

SceneObject::SceneObject() {
    this->modelMatrix = glm::mat4(1.0);
    this->boundingBoxMin = glm::vec3(FLT_MAX);
    this->boundingBoxMax = glm::vec3(FLT_MIN);
    this->mm = new MaterialManager();
}

SceneObject::~SceneObject() {
    std::cout << "<<<<< desctructor function of the SceneObject" << std::endl;
    for (std::vector<Mesh>::iterator it = this->meshes.begin(); it != this->meshes.end(); it++) {
        Mesh mesh = (*it);
        glDeleteBuffers(1, &mesh.vertexID);
        glDeleteBuffers(1, &mesh.normalID);
        glDeleteBuffers(1, &mesh.texcoordID);
        glDeleteBuffers(1, &mesh.tangentID);
        glDeleteBuffers(1, &mesh.bitangentID);
        glDeleteVertexArrays(1, &mesh.vaoID);
    }
    delete this->mm;
}

void SceneObject::addMesh(
        float *vertices, int numVertices,
        float *normals, int numNormals,
        float *textureCoordinates, int numTextureCoordinates,
        float *tangents, int numTangents,
        float *bitangents, int numBitangents,
        int *faces, int numFaces,
        const std::string &materialName) {

    int idx = mm->getMaterialIndex(materialName);

    this->addMesh(
            vertices, numVertices,
            normals, numNormals,
            textureCoordinates, numTextureCoordinates,
            tangents, numTangents,
            bitangents, numBitangents,
            faces, numFaces,
            idx);
}

void SceneObject::addMesh(
        float *vertices, int numVertices,
        float *normals, int numNormals,
        float *textureCoordinates, int numTextureCoordinates,
        float *tangents, int numTangents,
        float *bitangents, int numBitangents,
        int *faces, int numFaces,
        int materialIndex) {

    GLuint programID = (*ShaderManager::getSingleton())["phong"]->GetID();

    GLuint positionAttrib = glGetAttribLocation(programID, "in_position");
    GLuint normalAttrib = glGetAttribLocation(programID, "in_normal");
    GLuint texcoordAttrib = glGetAttribLocation(programID, "in_texcoord");
    GLuint tangentAttrib = glGetAttribLocation(programID, "in_tangent");
    GLuint bitangentAttrib = glGetAttribLocation(programID, "in_bitangent");

    Mesh m;

    m.material = materialIndex;
    m.boundingBoxMin = glm::vec3(FLT_MAX);
    m.boundingBoxMax = glm::vec3(FLT_MIN);

    Material *mat = mm->getMaterial(materialIndex);

    glGenVertexArrays(1, &m.vaoID);
    glBindVertexArray(m.vaoID);

    if (numVertices > 0) {
        for (int i = 0; i < numVertices; i++) {
            if (vertices[4 * i + 0] < this->boundingBoxMin.x)
                this->boundingBoxMin.x = vertices[4 * i + 0];
            if (vertices[4 * i + 0] > this->boundingBoxMax.x)
                this->boundingBoxMax.x = vertices[4 * i + 0];

            if (vertices[4 * i + 1] < this->boundingBoxMin.y)
                this->boundingBoxMin.y = vertices[4 * i + 1];
            if (vertices[4 * i + 1] > this->boundingBoxMax.y)
                this->boundingBoxMax.y = vertices[4 * i + 1];

            if (vertices[4 * i + 2] < this->boundingBoxMin.z)
                this->boundingBoxMin.z = vertices[4 * i + 2];
            if (vertices[4 * i + 2] > this->boundingBoxMax.z)
                this->boundingBoxMax.z = vertices[4 * i + 2];

            if (vertices[4 * i + 0] < m.boundingBoxMin.x)
                m.boundingBoxMin.x = vertices[4 * i + 0];
            if (vertices[4 * i + 0] > m.boundingBoxMax.x)
                m.boundingBoxMax.x = vertices[4 * i + 0];

            if (vertices[4 * i + 1] < m.boundingBoxMin.y)
                m.boundingBoxMin.y = vertices[4 * i + 1];
            if (vertices[4 * i + 1] > m.boundingBoxMax.y)
                m.boundingBoxMax.y = vertices[4 * i + 1];

            if (vertices[4 * i + 2] < m.boundingBoxMin.z)
                m.boundingBoxMin.z = vertices[4 * i + 2];
            if (vertices[4 * i + 2] > m.boundingBoxMax.z)
                m.boundingBoxMax.z = vertices[4 * i + 2];
        }

        glGenBuffers(1, &m.vertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m.vertexID);
        glBufferData(GL_ARRAY_BUFFER, numVertices * 4 * sizeof (GLfloat), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (numNormals > 0) {
        glGenBuffers(1, &m.normalID);
        glBindBuffer(GL_ARRAY_BUFFER, m.normalID);
        glBufferData(GL_ARRAY_BUFFER, numNormals * 3 * sizeof (GLfloat), normals, GL_STATIC_DRAW);
        glEnableVertexAttribArray(normalAttrib);
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (numTextureCoordinates > 0) {
        glGenBuffers(1, &m.texcoordID);
        glBindBuffer(GL_ARRAY_BUFFER, m.texcoordID);
        glBufferData(GL_ARRAY_BUFFER, numTextureCoordinates * 2 * sizeof (GLfloat), textureCoordinates, GL_STATIC_DRAW);
        glEnableVertexAttribArray(texcoordAttrib);
        glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (numTangents > 0) {
        glGenBuffers(1, &m.tangentID);
        glBindBuffer(GL_ARRAY_BUFFER, m.tangentID);
        glBufferData(GL_ARRAY_BUFFER, numTangents * 3 * sizeof (GLfloat), tangents, GL_STATIC_DRAW);
        glEnableVertexAttribArray(tangentAttrib);
        glVertexAttribPointer(tangentAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (numBitangents > 0) {
        glGenBuffers(1, &m.bitangentID);
        glBindBuffer(GL_ARRAY_BUFFER, m.bitangentID);
        glBufferData(GL_ARRAY_BUFFER, numBitangents * 3 * sizeof (GLfloat), bitangents, GL_STATIC_DRAW);
        glEnableVertexAttribArray(bitangentAttrib);
        glVertexAttribPointer(bitangentAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    m.faces = faces;
    m.numFaces = numFaces;

    glBindVertexArray(0);

    this->meshes.push_back(m);
}

void SceneObject::update(float dt) {

}

glm::vec3 SceneObject::getBoundingBoxMin() {
    return this->boundingBoxMin;
}

glm::vec3 SceneObject::getBoundingBoxMax() {
    return this->boundingBoxMax;
}

void SceneObject::render(GLuint programID) {
    for (std::vector<Mesh>::iterator it = this->meshes.begin(); it != this->meshes.end(); it++) {
        Mesh m = (*it);
        Material *mat = this->mm->getMaterial(m.material);
        mat->bindMaterial(programID);
        glBindVertexArray(m.vaoID);
        glDrawElements(GL_TRIANGLES, 3 * m.numFaces, GL_UNSIGNED_INT, m.faces);
        glBindVertexArray(0);
        mat->unbindMaterial();
    }
}

glm::mat4 SceneObject::getModelMatrix() {
    return this->modelMatrix;
}
