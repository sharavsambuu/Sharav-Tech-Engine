/* 
 * File:   main.cpp
 * Author: prompto
 *
 * Created on November 30, 2013, 4:48 PM
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_MESSAGES
#define GLM_FORCE_INLINE 
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <math.h>
#include <algorithm>
#include "MaterialManager.hpp"
#include "Material.hpp"
#include "ShaderManager.hpp"
#include "Shader.hpp"
#include "SceneObject.hpp"

using namespace std;


GLFWwindow *window;
int windowWidth = 1000;
int windowHeight = 800;

bool wKeyPressed;
bool sKeyPressed;
bool aKeyPressed;
bool dKeyPressed;
double mouseX, mouseY;
float speed = 150.09f;
float mouseSpeed = 0.04f;
glm::vec3 position = glm::vec3(0, 0, 5);
glm::vec3 direction;
//glm::vec3 right;
glm::vec3 up;
float horizontalAngle = 3.14159f;
float verticalAngle = 0.0f;
float fov = 60.0f;

glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

void initWindow();
void disposeWindow();
void handleKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(float dt);

void processMaterial(aiMaterial* material, int index, SceneObject *object);
void processMesh(aiMesh* mesh, SceneObject *object);

/*
 * experiment with ASSIMP
 */
int main(int argc, char** argv) {
    initWindow();
    glfwSetKeyCallback(window, handleKeyboardInput);
    ShaderManager* sm = ShaderManager::getSingleton();
    sm->CreateShaderProgram("phong");
    sm->AttachShader("phongVertex", VERTEX);
    sm->AttachShader("phongFragment", FRAGMENT);
    sm->LoadShaderSource("phongVertex", "shaders/pointlight.vert.glsl");
    sm->LoadShaderSource("phongFragment", "shaders/pointlight.frag.glsl");
    sm->CompileShader("phongVertex");
    sm->CompileShader("phongFragment");
    sm->AttachShaderToProgram("phong", "phongVertex");
    sm->AttachShaderToProgram("phong", "phongFragment");
    sm->LinkProgramObject("phong");
    std::cout << "ShaderProgram ID is " << (*sm)["phong"]->GetID() << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SceneObject *sceneObject = NULL;
    std::string filePath = "models/sponza.obj";
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
            filePath.c_str(),
            aiProcess_OptimizeGraph |
            aiProcess_OptimizeMeshes |
            aiProcess_ImproveCacheLocality |
            aiProcess_SplitLargeMeshes |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType
            );
    if (!scene) {
        std::cout << "error in loading file : " << importer.GetErrorString() << std::endl;
        return 1;
    }
    sceneObject = new SceneObject();
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        processMaterial(scene->mMaterials[i], i, sceneObject);
    }
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        processMesh(scene->mMeshes[i], sceneObject);
    }

    SceneObject *vehicleObject = NULL;
    //filePath = "models/phoenix_ugv.md2";
    filePath = "models/R8.obj";
    scene = importer.ReadFile(
            filePath.c_str(),
            aiProcess_OptimizeGraph |
            aiProcess_OptimizeMeshes |
            aiProcess_ImproveCacheLocality |
            aiProcess_SplitLargeMeshes |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType
            );
    if (!scene) {
        std::cout << "error in loading file : " << importer.GetErrorString() << std::endl;
        return 1;
    }
    vehicleObject = new SceneObject();
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        processMaterial(scene->mMaterials[i], i, vehicleObject);
    }
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        processMesh(scene->mMeshes[i], vehicleObject);
    }

    projectionMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0, 0, -2));
    glViewport(0, 0, (int) windowWidth, (int) windowHeight);
    projectionMatrix = glm::perspective(60.0f, (float) windowWidth / windowHeight, 1.0f, 10000.0f);
    glm::mat4 mvpMatrix;
    glm::mat3 normalMatrix;

    float ambientLightIntensity = 0.2;

    GLuint programID = (*sm)["phong"]->GetID();
    float deltaTime = 0.0f;
    float startTime = glfwGetTime();
    float buffTime = 0.0;
    int counter = 0;
    float animationTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        processInput(deltaTime);
        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightPosition = glm::vec3(360 * sin(animationTime), 40, 0);
        //glm::vec3 lightPosition = glm::vec3(0, 40, 40);
        static float lightRotation = 0.0f;
        lightRotation += 25.0f;

        glUseProgram(programID);
        glm::mat4 modelMatrix = sceneObject->getModelMatrix();
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
        normalMatrix = glm::inverseTranspose(glm::mat3(viewMatrix * modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(programID, "mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix3fv(glGetUniformLocation(programID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3f(glGetUniformLocation(programID, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform1f(glGetUniformLocation(programID, "ambientIntensity"), ambientLightIntensity);
        sceneObject->render(programID);
        glUseProgram(0);
        
        glUseProgram(programID);
        static float rotationAngle = 0.0f;
        rotationAngle += 5 * deltaTime;
        modelMatrix = vehicleObject->getModelMatrix();
        modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::vec3(0, 1, 0));
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
        normalMatrix = glm::inverseTranspose(glm::mat3(viewMatrix * modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(programID, "mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix3fv(glGetUniformLocation(programID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3f(glGetUniformLocation(programID, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform1f(glGetUniformLocation(programID, "ambientIntensity"), ambientLightIntensity);
        vehicleObject->render(programID);
        glUseProgram(0);
         
        glfwSwapBuffers(window);
        glfwPollEvents();
        double lastTime = glfwGetTime();
        deltaTime = lastTime - startTime;
        startTime = lastTime;
        counter++;
        buffTime += deltaTime;
        if (buffTime > 1.0f) {
            buffTime = 0.0f;
            cout << "fps " << counter << endl;
            counter = 0;
        }
        animationTime += deltaTime;
    }

    delete sceneObject;
    delete vehicleObject;
    delete ShaderManager::getSingleton();
    disposeWindow();
    return 0;
}

void processMesh(aiMesh* mesh, SceneObject *object) {
    float *normals, *textureCoordinates, *tangents, *bitangents;
    float *vertices = (float *) malloc(4 * mesh->mNumVertices * sizeof (float));
    if (mesh->HasNormals())
        normals = (float *) malloc(3 * mesh->mNumVertices * sizeof (float));
    if (mesh->HasTextureCoords(0))
        textureCoordinates = (float *) malloc(2 * mesh->mNumVertices * sizeof (float));
    if (mesh->HasTangentsAndBitangents()) {
        tangents = (float *) malloc(3 * mesh->mNumVertices * sizeof (float));
        bitangents = (float *) malloc(3 * mesh->mNumVertices * sizeof (float));
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices[4 * i + 0] = mesh->mVertices[i].x;
        vertices[4 * i + 1] = mesh->mVertices[i].y;
        vertices[4 * i + 2] = mesh->mVertices[i].z;
        vertices[4 * i + 3] = 1.0f;
        if (mesh->HasNormals()) {
            normals[3 * i + 0] = mesh->mNormals[i].x;
            normals[3 * i + 1] = mesh->mNormals[i].y;
            normals[3 * i + 2] = mesh->mNormals[i].z;
        }
        if (mesh->HasTextureCoords(0)) {
            textureCoordinates[2 * i + 0] = mesh->mTextureCoords[0][i].x;
            textureCoordinates[2 * i + 1] = mesh->mTextureCoords[0][i].y;
        }
        if (mesh->HasTangentsAndBitangents()) {
            tangents[3 * i + 0] = mesh->mTangents[i].x;
            tangents[3 * i + 1] = mesh->mTangents[i].y;
            tangents[3 * i + 2] = mesh->mTangents[i].z;
            bitangents[3 * i + 0] = mesh->mBitangents[i].x;
            bitangents[3 * i + 1] = mesh->mBitangents[i].y;
            bitangents[3 * i + 2] = mesh->mBitangents[i].z;
        }
    }
    int *faces = (int *) malloc(3 * mesh->mNumFaces * sizeof (int));
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        faces[3 * i + 0] = mesh->mFaces[i].mIndices[0];
        faces[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        faces[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }
    object->addMesh(
            vertices, mesh->mNumVertices,
            normals, mesh->mNumVertices,
            textureCoordinates, mesh->mNumVertices,
            tangents, mesh->mNumVertices,
            bitangents, mesh->mNumVertices,
            faces, mesh->mNumFaces,
            mesh->mMaterialIndex);
}

void processMaterial(aiMaterial* material, int index, SceneObject *object) {
    //MaterialManager *mm = MaterialManager::getSingleton();
    MaterialManager *mm = object->mm;
    std::cout << "================ ID " << index << "====================" << std::endl;

    aiString materialName;

    // Get the name for managing purpose
    if (material->Get(AI_MATKEY_NAME, materialName) != AI_SUCCESS) {
        std::cout << "ERROR: while getting name of material" << std::endl;
    }

    aiString diffuseTexturePath;
    aiString normalMapTexturePath;
    aiString specularTexturePath;
    aiString ambientTexturePath;
    aiString opacityTexturePath;
    aiColor3D diffuseColor, ambientColor, specularColor, emissiveColor;
    float opacity, shininess;

    // Get diffuse color
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get diffuse color for Material " << materialName.data << std::endl;
    }

    // Get ambient color
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get ambient color for Material " << materialName.data << std::endl;
    }

    // Get diffuse color
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get specular color for Material " << materialName.data << std::endl;
    }

    // Get opacity
    if (material->Get(AI_MATKEY_OPACITY, opacity) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get opacity for Material " << materialName.data << std::endl;
    }

    // Get emissive color
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) != AI_SUCCESS)
        emissiveColor = aiColor3D(0);

    // Get shininess
    if (material->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get Shininess for Material " << materialName.data << std::endl;
    }


    // material color parameters
    std::cout << "diffuse color [" << diffuseColor.r << "," << diffuseColor.g << "," << diffuseColor.b << "] opacity=" << opacity << std::endl;
    std::cout << "ambient color [" << ambientColor.r << "," << ambientColor.g << "," << ambientColor.b << "]" << std::endl;
    std::cout << "specular color [" << specularColor.r << "," << specularColor.g << "," << specularColor.b << "]" << std::endl;
    std::cout << "emissive color [" << emissiveColor.r << "," << emissiveColor.g << "," << emissiveColor.b << "]" << std::endl;


    Material *m = mm->createMaterial(std::string(materialName.data), index);
    m->setDiffuseColor(glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, opacity));
    m->setAmbientColor(glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b));
    m->setSpecularColor(glm::vec3(specularColor.r, specularColor.g, specularColor.b));
    m->setEmissiveColor(glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b));

    // diffuse map
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuseTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Diffuse Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasDiffuseTexture = true;
        m->loadDiffuseTexture(diffuseTexturePath.data);
        std::cout << "diffuse texture path " << diffuseTexturePath.data << std::endl;
    }

    // normal map
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), normalMapTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Height Map Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasNormalTexture = true;
        m->loadNormalTexture(normalMapTexturePath.data);
        std::cout << "height map texture path " << normalMapTexturePath.data << std::endl;
    }

    // specular map
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), specularTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Mask Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasSpecularTexture = true;
        m->loadSpecularTexture(specularTexturePath.data);
        std::cout << "specular texture path " << specularTexturePath.data << std::endl;
    } else {
        std::cout << "there is no specular texture at ID=" << index << std::endl;
    }

    // ambient map
    if (material->GetTextureCount(aiTextureType_AMBIENT) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), ambientTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Ambient Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasAmbientTexture = true;
        m->loadAmbientTexture(ambientTexturePath.data);
        std::cout << "ambient texture path " << ambientTexturePath.data << std::endl;
    } else {
        std::cout << ":( THERE IS NO AMBIENT MAP" << std::endl;
    }
    // opacity map
    if (material->GetTextureCount(aiTextureType_OPACITY) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_OPACITY, 0), opacityTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Couldn't get opacity texture for material" << materialName.data << std::endl;
            return;
        }
        m->hasOpacityTexture = true;
        m->loadOpacityTexture(opacityTexturePath.data);
        std::cout << "opacity texture path" << opacityTexturePath.data << std::endl;
    } else {
        std::cout << ":( THERE IS NO OPACITY MAP" << std::endl;
    }
    std::cout << "==========================================" << std::endl;
}

void initWindow() {
    if (!glfwInit()) {
        std::cout << "Error: GLFW failed to initialize.\n";
        return;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL ASSIMP", NULL, NULL);
    if (!window) {
        std::cout << "Error: Failed to create window.\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        std::cout << "Error: Failed to load OpenGL functions.\n";
        glfwTerminate();
        return;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void disposeWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void handleKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
        wKeyPressed = true;
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        sKeyPressed = true;
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        aKeyPressed = true;
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        dKeyPressed = true;
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        wKeyPressed = false;
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        sKeyPressed = false;
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        aKeyPressed = false;
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        dKeyPressed = false;
}

void processInput(float dt) {
    static glm::vec3 right;
    if (wKeyPressed)
        position += direction * speed * dt;
    if (sKeyPressed)
        position -= direction * speed * dt;
    if (aKeyPressed)
        position -= right * speed * dt;
    if (dKeyPressed)
        position += right * speed * dt;

    direction = glm::vec3
            (
            std::cos(verticalAngle) * std::sin(horizontalAngle),
            std::sin(verticalAngle),
            std::cos(verticalAngle) * std::cos(horizontalAngle)
            );
    right = glm::vec3
            (
            std::sin(horizontalAngle - 3.14159f / 2.0f),
            0.0f,
            std::cos(horizontalAngle - 3.14159f / 2.0f)
            );
    up = glm::cross(right, direction);

    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
    horizontalAngle += mouseSpeed * dt * float(windowWidth / 2 - mouseX);
    if (!up.y >= -0.5f)
        verticalAngle += mouseSpeed * dt * float(windowHeight / 2 - mouseY);
    else if (up.y == -1.0f)
        up = glm::vec3(up.x, 1, up.z);

    viewMatrix = glm::lookAt(position, position + direction, up);
}