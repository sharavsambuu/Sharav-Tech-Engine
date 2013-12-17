
#include "DefferedRenderingExperiment.hpp"
#include "GLFWApp.hpp"
#include "Camera.hpp"
#include <iostream>
#include "ModelLoader.hpp"
#include "Common.hpp"
#include "Gear.hpp"
#include "GLError.hpp"
#include "PointLight.hpp"
#include <glm/gtx/norm.hpp> 

DefferedRenderingExperiment::DefferedRenderingExperiment() {
    isEntered = true;
    isFirstTime = true;
    isInitialized = false;
}

DefferedRenderingExperiment::~DefferedRenderingExperiment() {
    std::cout << "<<<<< Dof experiment destruction function " << std::endl;
    for (auto object : sceneObjects) {
        if (object)
            delete object;
    }
    delete pointLightVolume;
    glDeleteTextures(1, &colourTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteFramebuffers(1, &gbufferFBO);
    glDeleteTextures(1, &emissiveTexture);
    glDeleteTextures(1, &specularTexture);
    glDeleteFramebuffers(1, &lightingFBO);
    glDeleteBuffers(1, &quadBufferID);
    delete ShaderManager::getSingleton();
}

bool DefferedRenderingExperiment::isInitializationDone() {
    return this->isInitialized;
}

void DefferedRenderingExperiment::pause() {
    isEntered = false;
}

void DefferedRenderingExperiment::resume() {
    isEntered = true;
    enteredTime = 0.0f;
    std::cout << "<<<<< Entered Deffered Rendering experimentation State" << std::endl;
    if (isFirstTime) {
        isInitialized = false;
        initialize();
        isFirstTime = false;
    }
}

void DefferedRenderingExperiment::input() {
    GLFWApp *app = GLFWApp::getSingleton();

    camera->updateAngles(app->getDeltaMouseX(), app->getDeltaMouseY());

    if (app->getKeyPress(GLFW_KEY_W))
        this->wKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_W))
        this->wKeyPressed = false;

    if (app->getKeyPress(GLFW_KEY_A))
        this->aKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_A))
        this->aKeyPressed = false;

    if (app->getKeyPress(GLFW_KEY_S))
        this->sKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_S))
        this->sKeyPressed = false;

    if (app->getKeyPress(GLFW_KEY_D))
        this->dKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_D))
        this->dKeyPressed = false;

}

void DefferedRenderingExperiment::update(float deltaTime) {
    camera->moveForward(false);
    camera->moveBackward(false);
    camera->moveLeft(false);
    camera->moveRight(false);
    if (wKeyPressed)
        camera->moveForward(true);
    if (sKeyPressed)
        camera->moveBackward(true);
    if (aKeyPressed)
        camera->moveLeft(true);
    if (dKeyPressed)
        camera->moveRight(true);

    this->camera->update(deltaTime);

    this->viewMatrix = camera->getViewMatrix();
    this->projectionMatrix = camera->getProjectionMatrix();

    for (AbstractLight* light : sceneLights) {
        light->update(deltaTime);
    }
    for (AbstractSceneObject* sceneObject : sceneObjects) {
        sceneObject->update(deltaTime);
    }
}

void DefferedRenderingExperiment::render() {
    //************************** FILL GBUFFER **********************************
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);

    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (AbstractSceneObject* sceneObject : sceneObjects) {
        glUseProgram(gbufferProgramID);
        glm::mat4 modelMatrix = sceneObject->getModelMatrix();
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
        normalMatrix = glm::inverseTranspose(glm::mat3(viewMatrix * modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(gbufferProgramID, "mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(glGetUniformLocation(gbufferProgramID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(gbufferProgramID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix3fv(glGetUniformLocation(gbufferProgramID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        sceneObject->render(gbufferProgramID);
        glUseProgram(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //**************************** LIGHTING **********************************
    glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
    glEnable(GL_STENCIL_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_ONE, GL_ONE);
    for (AbstractLight* light : sceneLights) {
        glm::mat4 lightModelMatrix = glm::mat4(1.0);
        lightModelMatrix = glm::translate(lightModelMatrix, light->getPosition());
        lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(light->getRadius(), light->getRadius(), light->getRadius()));
        pointLightVolume->setModelMatrix(lightModelMatrix);
        float distance = glm::length(light->getPosition() - camera->getPosition());
        if (distance < light->getRadius()) {
            glCullFace(GL_FRONT);
        } else {
            glCullFace(GL_BACK);
        }
        glUseProgram(lightingProgramID);
        glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colourTexture);
        glUniform1i(glGetUniformLocation(lightingProgramID, "colour_texture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glUniform1i(glGetUniformLocation(lightingProgramID, "normal_texture"), 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glUniform1i(glGetUniformLocation(lightingProgramID, "depth_texture"), 2);

        glUniform2f(glGetUniformLocation(lightingProgramID, "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);
        glUniform1i(glGetUniformLocation(lightingProgramID, "lightRadius"), light->getRadius());
        glUniform3f(glGetUniformLocation(lightingProgramID, "lightPos"), light->getPosition().x, light->getPosition().y, light->getPosition().z);
        glUniform4f(glGetUniformLocation(lightingProgramID, "lightColor"), light->getColor().x, light->getColor().y, light->getColor().z, light->getColor().w);
        glUniform3f(glGetUniformLocation(lightingProgramID, "cameraPos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
        glUniformMatrix4fv(glGetUniformLocation(lightingProgramID, "inverseProjection"), 1, GL_FALSE, glm::value_ptr(inverseProjectionMatrix));
        glUniform2f(glGetUniformLocation(lightingProgramID, "screen_dimension"), windowWidth, windowHeight);
        glm::mat4 modelMatrix = pointLightVolume->getModelMatrix();
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
        normalMatrix = glm::inverseTranspose(glm::mat3(viewMatrix * modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(lightingProgramID, "mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(glGetUniformLocation(lightingProgramID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(lightingProgramID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix3fv(glGetUniformLocation(lightingProgramID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        pointLightVolume->render(lightingProgramID);
        glUseProgram(0);
    }
    glDisable(GL_STENCIL_TEST);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //************************* PRESENTING FRAME *******************************
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(combineProgramID);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colourTexture);
    glUniform1i(glGetUniformLocation(combineProgramID, "diffuseTexture"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture);
    glUniform1i(glGetUniformLocation(combineProgramID, "emissiveTexture"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularTexture);
    glUniform1i(glGetUniformLocation(combineProgramID, "specularTexture"), 2);
    
    glUniform2f(glGetUniformLocation(combineProgramID, "screen_dimension"), windowWidth, windowHeight);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

void DefferedRenderingExperiment::initialize() {
    this->windowWidth = (int) SCREEN_WIDTH;
    this->windowHeight = (int) SCREEN_HEIGHT;

    camera = new Camera();

    ShaderManager* sm = ShaderManager::getSingleton();
    sm->isUsingGBuffer = true;

    sm->CreateShaderProgram("gbuffer");
    sm->AttachShader("gbufferVertex", VERTEX);
    sm->AttachShader("gbufferFragment", FRAGMENT);
    sm->LoadShaderSource("gbufferVertex", "shaders/gbuffer.vert.glsl");
    sm->LoadShaderSource("gbufferFragment", "shaders/gbuffer.frag.glsl");
    sm->CompileShader("gbufferVertex");
    sm->CompileShader("gbufferFragment");
    sm->AttachShaderToProgram("gbuffer", "gbufferVertex");
    sm->AttachShaderToProgram("gbuffer", "gbufferFragment");
    sm->LinkProgramObject("gbuffer");
    std::cout << "GBuffer Program ID is " << (*sm)["gbuffer"]->GetID() << std::endl;

    sm->CreateShaderProgram("lighting");
    sm->AttachShader("lightingVertex", VERTEX);
    sm->AttachShader("lightingFragment", FRAGMENT);
    sm->LoadShaderSource("lightingVertex", "shaders/lightingpass.vert.glsl");
    sm->LoadShaderSource("lightingFragment", "shaders/lightingpass.frag.glsl");
    sm->CompileShader("lightingVertex");
    sm->CompileShader("lightingFragment");
    sm->AttachShaderToProgram("lighting", "lightingVertex");
    sm->AttachShaderToProgram("lighting", "lightingFragment");
    sm->LinkProgramObject("lighting");
    std::cout << "Lighting pass Program ID is " << (*sm)["lighting"]->GetID() << std::endl;

    sm->CreateShaderProgram("combine");
    sm->AttachShader("combineVertex", VERTEX);
    sm->AttachShader("combineFragment", FRAGMENT);
    sm->LoadShaderSource("combineVertex", "shaders/gcombine.vert.glsl");
    sm->LoadShaderSource("combineFragment", "shaders/gcombine.frag.glsl");
    sm->CompileShader("combineVertex");
    sm->CompileShader("combineFragment");
    sm->AttachShaderToProgram("combine", "combineVertex");
    sm->AttachShaderToProgram("combine", "combineFragment");
    sm->LinkProgramObject("combine");
    std::cout << "combine program ID is " << (*sm)["combine"]->GetID() << std::endl;

    // First pass stuffs

    glGenTextures(1, &colourTexture);
    glBindTexture(GL_TEXTURE_2D, colourTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glGenFramebuffers(1, &gbufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    GLenum buffers[2];
    buffers[0] = GL_COLOR_ATTACHMENT0;
    buffers[1] = GL_COLOR_ATTACHMENT1;

    glDrawBuffers(2, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << ">>>>> cannot create a framebuffer object for the gbuffer" << std::endl;
        check_gl_error();
        Gear::getSingleton()->exit();
        isInitialized = false;
        return;
    } else {
        std::cout << "it seems like FBO is created for gbuffer and it's good to go" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Second pass stuffs

    glGenTextures(1, &emissiveTexture);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenTextures(1, &specularTexture);
    glBindTexture(GL_TEXTURE_2D, specularTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenFramebuffers(1, &lightingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, emissiveTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, specularTexture, 0);

    glDrawBuffers(2, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << ">>>>> cannot create a framebuffer object for the lighting" << std::endl;
        check_gl_error();
        Gear::getSingleton()->exit();
        isInitialized = false;
        return;
    } else {
        std::cout << "it seems like FBO is created for the lighting and it's good to go" << std::endl;
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

    // Loading scene stuff

    ModelLoader modelLoader;
    //SceneObject *sponzaObject = new SceneObject();
    //modelLoader.loadSceneModel("models/sponza.obj", sponzaObject);
    //sceneObjects.push_back(sponzaObject);
    SceneObject *vehicleObject = new SceneObject();
    modelLoader.loadSceneModel("models/R8.obj", vehicleObject);
    glm::mat4 vehicleModelMatrix = vehicleObject->getModelMatrix();
    vehicleObject->setModelMatrix(glm::scale(vehicleObject->getModelMatrix(), glm::vec3(10, 10, 10)));
    sceneObjects.push_back(vehicleObject);

    pointLightVolume = new SceneObject();
    pointLightVolume->setLightVolumeBool(true);
    modelLoader.loadSceneModel("models/sphere.obj", pointLightVolume);

    // Creating lights
    //PointLight *pointLight0 = PointLight();
    for (int i=0; i<50; i++)
        sceneLights.push_back(new PointLight());
    



    glViewport(0, 0, (int) windowWidth, (int) windowHeight);

    gbufferProgramID = (*sm)["gbuffer"]->GetID();
    lightingProgramID = (*sm)["lighting"]->GetID();
    combineProgramID = (*sm)["combine"]->GetID();

    this->isInitialized = true;
}
