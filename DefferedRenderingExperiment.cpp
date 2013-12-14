
#include "DefferedRenderingExperiment.hpp"
#include "GLFWApp.hpp"
#include "Camera.hpp"
#include <iostream>
#include "ModelLoader.hpp"
#include "Common.hpp"
#include "Gear.hpp"
#include "GLError.hpp"
#include <thread>

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
    glDeleteTextures(1, &colourTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteFramebuffers(1, &gbufferFBO);
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

}

void DefferedRenderingExperiment::render() {
    //********************* DRAWING SCENE TO THE FBO ***********************
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);

    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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
    //******************** END OF THE DRAWING SCENE ************************

    glEnable(GL_DEPTH_TEST);

    // TO PRESENT PROCESSED FRAME TO THE SCREEN
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(quadProgramID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(glGetUniformLocation(quadProgramID, "uScreenTex"), 0);
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

    // let's create some textures needed for G-Buffer

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    glGenFramebuffers(1, &gbufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    GLenum buffers[2];
    buffers[0] = GL_COLOR_ATTACHMENT0;
    buffers[1] = GL_COLOR_ATTACHMENT1;

    glDrawBuffers(2, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << ">>>>> cannot create a framebuffer object" << std::endl;
        check_gl_error();
        Gear::getSingleton()->exit();
        isInitialized = false;
        return;
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

    ModelLoader modelLoader;
    SceneObject *sponzaObject = new SceneObject();
    modelLoader.loadSceneModel("models/sponza.obj", sponzaObject);
    sceneObjects.push_back(sponzaObject);
    SceneObject *vehicleObject = new SceneObject();
    modelLoader.loadSceneModel("models/R8.obj", vehicleObject);
    glm::mat4 vehicleModelMatrix = vehicleObject->getModelMatrix();
    vehicleObject->setModelMatrix(glm::scale(vehicleObject->getModelMatrix(), glm::vec3(10, 10, 10)));
    this->sceneObjects.push_back(vehicleObject);

    ////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, (int) windowWidth, (int) windowHeight);

    gbufferProgramID = (*sm)["gbuffer"]->GetID();
    quadProgramID = (*sm)["quad"]->GetID();

    this->isInitialized = true;
}
