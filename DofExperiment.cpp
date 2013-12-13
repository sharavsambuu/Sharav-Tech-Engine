#include "DofExperiment.hpp"
#include "GLFWApp.hpp"
#include "Camera.hpp"
#include <iostream>
#include "ModelLoader.hpp"
#include "Common.hpp"
#include "Gear.hpp"
#include "GLError.hpp"
#include <thread>

DofExperiment::DofExperiment() {
    isEntered = true;
    isFirstTime = true;
    isInitialized = false;
}

DofExperiment::~DofExperiment() {
    std::cout << "<<<<< Dof experiment destruction function " << std::endl;
    for (auto object : sceneObjects) {
        if (object)
            delete object;
    }
    delete ShaderManager::getSingleton();
}

bool DofExperiment::isInitializationDone() {
    return this->isInitialized;
}

void DofExperiment::pause() {
    isEntered = false;
}

void DofExperiment::resume() {
    isEntered = true;
    enteredTime = 0.0f;
    std::cout << "<<<<< Entered DoF experimentation State" << std::endl;
    if (isFirstTime) {
        isInitialized = false;
        initialize();
        //std::thread initThread(&DofExperiment::initialize, DofExperiment());
        //initThread.join();
        isFirstTime = false;
    }
}

void DofExperiment::input() {
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

void DofExperiment::update(float deltaTime) {
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

void DofExperiment::render() {
    //********************* DRAWING SCENE TO THE FBO ***********************
    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);

    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    projectionMatrix = glm::perspective(60.0f, (float) windowWidth / windowHeight, 1.0f, 10000.0f);

    glm::vec3 lightPosition = glm::vec3(360 * sin(animationTime), 40, 0);

    std::cout << "<<<<< object count : " << this->sceneObjects.size() << std::endl;

    /*
        for (int i = 0; i<this->sceneObjects.size(); i++) {
            SceneObject *sceneObject = sceneObjects[i];
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
        }
     */
    /*
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
     */

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
}

void DofExperiment::initialize() {
    this->windowWidth = (int) SCREEN_WIDTH;
    this->windowHeight = (int) SCREEN_HEIGHT;
    camera = new Camera();
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
    //SceneObject *sponzaObject = new SceneObject();
    //modelLoader.loadSceneModel("models/sponza.obj", sponzaObject);
    //sceneObjects.push_back(sponzaObject);
    SceneObject *vehicleObject = new SceneObject();
    modelLoader.loadSceneModel("models/R8.obj", vehicleObject);
    if (vehicleObject==NULL) {
        std::cout<<"shit it's null"<<std::endl;
        Gear::getSingleton()->exit();
    }
    this->sceneObjects.push_back(vehicleObject);
    
    
    ////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, (int) windowWidth, (int) windowHeight);

    ambientLightIntensity = 0.2;

    programID = (*sm)["phong"]->GetID();
    quadProgramID = (*sm)["quad"]->GetID();
    gausProgramID = (*sm)["gaus"]->GetID();
    dofProgramID = (*sm)["dof"]->GetID();

    animationTime = 0.0f;

    this->isInitialized = true;
    //return true; // initialization is done
}
