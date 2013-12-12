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
#include "ModelLoader.hpp"
#include <SOIL.h>

using namespace std;


GLFWwindow *window;
int windowWidth = 1000;
int windowHeight = 800;

bool wKeyPressed;
bool sKeyPressed;
bool aKeyPressed;
bool dKeyPressed;

bool doGaus = false;
bool doDOF = true;


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

// frame buffer object stuff
GLuint bufferFBO;
GLuint processFBO;
GLuint colourTexture[2];
GLuint depthTexture;

void initWindow();
void disposeWindow();
void handleKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(float dt);


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
    std::cout << "Phong Program ID is " << (*sm)["phong"]->GetID() << std::endl;

    sm->CreateShaderProgram("quad");
    sm->AttachShader("quadVertex", VERTEX);
    sm->AttachShader("quadFragment", FRAGMENT);
    sm->LoadShaderSource("quadVertex", "shaders/quad.vert.glsl");
    sm->LoadShaderSource("quadFragment", "shaders/quad.frag.glsl");
    sm->CompileShader("quadVertex");
    sm->CompileShader("quadFragment");
    sm->AttachShaderToProgram("quad", "quadVertex");
    sm->AttachShaderToProgram("quad", "quadFragment");
    sm->LinkProgramObject("quad");
    std::cout << "Quad Program ID is " << (*sm)["quad"]->GetID() << std::endl;

    sm->CreateShaderProgram("gaus");
    sm->AttachShader("gausVertex", VERTEX);
    sm->AttachShader("gausFragment", FRAGMENT);
    sm->LoadShaderSource("gausVertex", "shaders/gaus.vert.glsl");
    sm->LoadShaderSource("gausFragment", "shaders/gaus.frag.glsl");
    sm->CompileShader("gausVertex");
    sm->CompileShader("gausFragment");
    sm->AttachShaderToProgram("gaus", "gausVertex");
    sm->AttachShaderToProgram("gaus", "gausFragment");
    sm->LinkProgramObject("gaus");
    std::cout << "Gaus ID is " << (*sm)["gaus"]->GetID() << std::endl;

    sm->CreateShaderProgram("dof");
    sm->AttachShader("dofVertex", VERTEX);
    sm->AttachShader("dofFragment", FRAGMENT);
    sm->LoadShaderSource("dofVertex", "shaders/dof.vert.glsl");
    sm->LoadShaderSource("dofFragment", "shaders/dof.frag.glsl");
    sm->CompileShader("dofVertex");
    sm->CompileShader("dofFragment");
    sm->AttachShaderToProgram("dof", "dofVertex");
    sm->AttachShaderToProgram("dof", "dofFragment");
    sm->LinkProgramObject("dof");
    std::cout << "DOF ID is " << (*sm)["dof"]->GetID() << std::endl;

    SceneObject *sceneObject = new SceneObject();
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
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        processMaterial(scene->mMaterials[i], i, sceneObject);
    }
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        processMesh(scene->mMeshes[i], sceneObject);
    }

    SceneObject *vehicleObject = new SceneObject();
    //filePath = "models/phoenix_ugv.md2";
    //filePath = "models/jeepwrangler.obj";
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
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        processMaterial(scene->mMaterials[i], i, vehicleObject);
    }
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        processMesh(scene->mMeshes[i], vehicleObject);
    }

    /////////////////////////////   COOKING   //////////////////////////////////
    // test cooking area
    // 
    // end of test cooking area


    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    for (int i = 0; i < 2; i++) {
        glGenTextures(1, &colourTexture[i]);
        glBindTexture(GL_TEXTURE_2D, colourTexture[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    glGenFramebuffers(1, &bufferFBO);
    glGenFramebuffers(1, &processFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture[0], 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "cannot create a framebuffer object" << std::endl;
        exit(-1);
    } else {
        std::cout << "it seems like FBO is created and it's good to go" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // screen aligned quads
    const GLfloat quadVertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };
    GLuint quadBufferID;
    glGenBuffers(2, &quadBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof (quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ////////////////////////////////////////////////////////////////////////////


    projectionMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0, 0, -2));
    glViewport(0, 0, (int) windowWidth, (int) windowHeight);
    projectionMatrix = glm::perspective(60.0f, (float) windowWidth / windowHeight, 1.0f, 10000.0f);
    glm::mat4 mvpMatrix;
    glm::mat3 normalMatrix;

    float ambientLightIntensity = 0.2;

    GLuint programID = (*sm)["phong"]->GetID();
    GLuint quadProgramID = (*sm)["quad"]->GetID();
    GLuint gausProgramID = (*sm)["gaus"]->GetID();
    GLuint dofProgramID = (*sm)["dof"]->GetID();

    float deltaTime = 0.0f;
    float startTime = glfwGetTime();
    float buffTime = 0.0;
    int counter = 0;
    float animationTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        processInput(deltaTime);

        //********************* DRAWING SCENE TO THE FBO ***********************
        glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);

        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        projectionMatrix = glm::perspective(60.0f, (float) windowWidth / windowHeight, 1.0f, 10000.0f);

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
        modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 10, 10));
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //******************** END OF THE DRAWING SCENE ************************

        //******************** STARTING OF POST PROCESSING *********************
        if (doGaus == true) {
            for (int i = 0; i < 10; i++) {
                // PING PONGING between attachments
                glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture[1], 0);
                glViewport(0, 0, windowWidth, windowHeight);
                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST);
                glUseProgram(gausProgramID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, colourTexture[0]);
                glUniform1i(glGetUniformLocation(gausProgramID, "uScreenTex"), 0);
                glUniform1i(glGetUniformLocation(gausProgramID, "isVertical"), 1);
                glUniform2f(glGetUniformLocation(gausProgramID, "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(0);
                glUseProgram(0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture[0], 0);
                glUseProgram(gausProgramID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, colourTexture[1]);
                glUniform1i(glGetUniformLocation(gausProgramID, "uScreenTex"), 0);
                glUniform1i(glGetUniformLocation(gausProgramID, "isVertical"), 0);
                glUniform2f(glGetUniformLocation(gausProgramID, "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(0);
                glUseProgram(0);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
        if (doDOF == true) {
            glm::mat4 projectionInverseMatrix = glm::inverse(projectionMatrix);
            for (int i = 0; i < 1; i++) {
                glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture[1], 0);
                glViewport(0, 0, windowWidth, windowHeight);
                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST);
                glUseProgram(dofProgramID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, colourTexture[0]);
                glUniform1i(glGetUniformLocation(dofProgramID, "uScreenTex"), 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, depthTexture);
                glUniform1i(glGetUniformLocation(dofProgramID, "uDepthTex"), 1);
                glUniform1i(glGetUniformLocation(dofProgramID, "isVertical"), 1);
                glUniform2f(glGetUniformLocation(dofProgramID, "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);
                glUniform1f(glGetUniformLocation(dofProgramID, "focus"), 360.0f * sin(animationTime));
                glUniformMatrix4fv(glGetUniformLocation(dofProgramID, "projectionInverseMatrix"), 1, GL_FALSE, glm::value_ptr(projectionInverseMatrix));
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(0);
                glUseProgram(0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture[0], 0);
                glUseProgram(dofProgramID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, colourTexture[1]);
                glUniform1i(glGetUniformLocation(dofProgramID, "uScreenTex"), 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, depthTexture);
                glUniform1i(glGetUniformLocation(dofProgramID, "uDepthTex"), 1);
                glUniform1i(glGetUniformLocation(dofProgramID, "isVertical"), 0);
                glUniform2f(glGetUniformLocation(dofProgramID, "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);
                glUniform1f(glGetUniformLocation(dofProgramID, "focus"), 360.0f * sin(animationTime));
                glUniformMatrix4fv(glGetUniformLocation(dofProgramID, "projectionInverseMatrix"), 1, GL_FALSE, glm::value_ptr(projectionInverseMatrix));
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDisableVertexAttribArray(0);
                glUseProgram(0);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
        //******************** END OF THE POST PROCESSING **********************

        glEnable(GL_DEPTH_TEST);

        // TO PRESENT PROCESSED FRAME TO THE SCREEN
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glUseProgram(quadProgramID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colourTexture[0]);
        //glBindTexture(GL_TEXTURE_2D, depthTexture);
        glUniform1i(glGetUniformLocation(quadProgramID, "uScreenTex"), 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
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
    glDeleteFramebuffers(1, &bufferFBO);
    glDeleteFramebuffers(1, &processFBO);
    glDeleteTextures(2, colourTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteBuffers(1, &quadBufferID);
    delete ShaderManager::getSingleton();
    disposeWindow();
    return 0;
}

void initWindow() {
    if (!glfwInit()) {
        std::cout << "Error: GLFW failed to initialize.\n";
        return;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "STE", NULL, NULL);
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
    else if (key == GLFW_KEY_1 && action == GLFW_RELEASE)
        doGaus = !doGaus;
    else if (key == GLFW_KEY_2 && action == GLFW_RELEASE)
        doDOF = !doDOF;
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