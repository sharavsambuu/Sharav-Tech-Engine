#include "DeferredExperiment.hpp"
#include "GLFWApp.hpp"
#include "Camera.hpp"
#include <iostream>
#include "ModelLoader.hpp"
#include "Common.hpp"
#include "Gear.hpp"
#include "GLError.hpp"
#include "PointLight.hpp"
#include <glm/gtx/norm.hpp>
#include <GL/glcorearb.h> 

DeferredExperiment::DeferredExperiment() {
    isEntered = true;
    isFirstTime = true;
    isInitialized = false;
}

DeferredExperiment::~DeferredExperiment() {
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

bool DeferredExperiment::isInitializationDone() {
    return this->isInitialized;
}

void DeferredExperiment::pause() {
    isEntered = false;
}

void DeferredExperiment::resume() {
    isEntered = true;
    enteredTime = 0.0f;
    std::cout << "<<<<< Entered Deffered Rendering experimentation State" << std::endl;
    if (isFirstTime) {
        isInitialized = false;
        initialize();
        isFirstTime = false;
    }
}

void DeferredExperiment::input() {
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

void DeferredExperiment::update(float deltaTime) {
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

    this->viewMatrix              = camera->getViewMatrix();
    this->projectionMatrix        = camera->getProjectionMatrix();
    this->inverseProjectionMatrix = glm::inverse(projectionMatrix);

    for (AbstractLight* light : sceneLights) {
        light->update(deltaTime);
    }
    for (AbstractSceneObject* sceneObject : sceneObjects) {
        sceneObject->update(deltaTime);
    }    
}

void DeferredExperiment::render() {
    //************************** FILL GBUFFER **********************************
    
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    
    for (AbstractSceneObject* sceneObject : sceneObjects) {
        glUseProgram(gbufferProgramID);
        glm::mat4 worldMatrix = sceneObject->getModelMatrix();
        glUniformMatrix4fv(glGetUniformLocation(gbufferProgramID, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(glGetUniformLocation(gbufferProgramID, "u_View"      ), 1, GL_FALSE, glm::value_ptr(viewMatrix      ));
        glUniformMatrix4fv(glGetUniformLocation(gbufferProgramID, "u_World"     ), 1, GL_FALSE, glm::value_ptr(worldMatrix     ));
        sceneObject->render(gbufferProgramID);
        glUseProgram(0);
    }
    
    //**************************** LIGHTING ************************************
    
    glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
    /*GLenum lightingBuffers[2];
    lightingBuffers[0] = GL_COLOR_ATTACHMENT0;
    lightingBuffers[1] = GL_COLOR_ATTACHMENT1;
    glDrawBuffers(2, lightingBuffers);*/
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_GEQUAL);
    
    glUseProgram(lightingProgramID);
    
    glUniformMatrix4fv(glGetUniformLocation(lightingProgramID, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(lightingProgramID, "u_View"      ), 1, GL_FALSE, glm::value_ptr(viewMatrix      ));
    for (AbstractLight* light : sceneLights) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colourTexture);
        glUniform1i(glGetUniformLocation(lightingProgramID, "u_DiffuseTex"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glUniform1i(glGetUniformLocation(lightingProgramID, "u_NormalTex"), 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glUniform1i(glGetUniformLocation(lightingProgramID, "u_DepthTex"), 2);
        
        glm::vec4 posInView = viewMatrix * glm::vec4(light->getPosition(), 1.0f);
        glm::mat4 worldLightMat(1);
        worldLightMat = glm::scale(worldLightMat, glm::vec3(light->getRadius(), light->getRadius(), light->getRadius()));
        glUniformMatrix4fv(glGetUniformLocation(gbufferProgramID, "u_World"     ), 1, GL_FALSE, glm::value_ptr(worldLightMat));
        glUniformMatrix4fv(glGetUniformLocation(lightingProgramID, "u_InvProj"), 1, GL_FALSE, glm::value_ptr(inverseProjectionMatrix));
        glUniform2f(glGetUniformLocation(lightingProgramID, "u_Viewport"), (float)windowWidth, (float)windowHeight);
        glUniform3f(glGetUniformLocation(lightingProgramID, "u_LightColor"), light->getColor().x, light->getColor().y, light->getColor().z);
        glUniform3f(glGetUniformLocation(lightingProgramID, "u_LightPosition"), posInView.x, posInView.y, posInView.z);
        glUniform1f(glGetUniformLocation(lightingProgramID, "u_LightSize"), light->getRadius());

        /*
        glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices)/sizeof(cubeIndices[0]), GL_UNSIGNED_SHORT, NULL);
        */
        
        glUniform2f(glGetUniformLocation(lightingProgramID, "u_Viewport"), (float)windowWidth, (float)windowHeight);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
       
    }    
    
    /*
    glUniform2f(glGetUniformLocation(lightingProgramID, "u_Viewport"), (float)windowWidth, (float)windowHeight);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    */
        
    glActiveTexture(0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    

    //************************* PRESENTING FRAME *******************************
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    glUseProgram(combineProgramID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture);
    glUniform1i(glGetUniformLocation(combineProgramID, "diffuseTexture"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture);
    glUniform1i(glGetUniformLocation(combineProgramID, "emissiveTexture"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularTexture);
    glUniform1i(glGetUniformLocation(combineProgramID, "specularTexture"), 2);
    
    glUniform2f(glGetUniformLocation(combineProgramID, "screen_dimension"), (float)windowWidth, (float)windowHeight);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);

    glUseProgram(0);
}

void DeferredExperiment::initialize() {
    this->windowWidth = (int) SCREEN_WIDTH;
    this->windowHeight = (int) SCREEN_HEIGHT;

    camera = new Camera();

    ShaderManager* sm = ShaderManager::getSingleton();
    sm->isUsingGBuffer = true;

    sm->CreateShaderProgram("gbuffer");
    sm->AttachShader("gbufferVertex"  , VERTEX  );
    sm->AttachShader("gbufferFragment", FRAGMENT);
    sm->LoadShaderSource("gbufferVertex"  , "shaders/gbuffer.vert.glsl");
    sm->LoadShaderSource("gbufferFragment", "shaders/gbuffer.frag.glsl");
    sm->CompileShader("gbufferVertex"  );
    sm->CompileShader("gbufferFragment");
    sm->AttachShaderToProgram("gbuffer", "gbufferVertex"  );
    sm->AttachShaderToProgram("gbuffer", "gbufferFragment");
    sm->LinkProgramObject("gbuffer");
    std::cout << "GBuffer Program ID is " << (*sm)["gbuffer"]->GetID() << std::endl;

    sm->CreateShaderProgram("lighting");
    sm->AttachShader("lightingVertex"  , VERTEX  );
    sm->AttachShader("lightingFragment", FRAGMENT);
    sm->LoadShaderSource("lightingVertex"  , "shaders/lightingpass.vert.glsl");
    sm->LoadShaderSource("lightingFragment", "shaders/lightingpass.frag.glsl");
    sm->CompileShader("lightingVertex"  );
    sm->CompileShader("lightingFragment");
    sm->AttachShaderToProgram("lighting", "lightingVertex"  );
    sm->AttachShaderToProgram("lighting", "lightingFragment");
    sm->LinkProgramObject("lighting");
    std::cout << "Lighting pass Program ID is " << (*sm)["lighting"]->GetID() << std::endl;

    sm->CreateShaderProgram("combine");
    sm->AttachShader("combineVertex"  , VERTEX  );
    sm->AttachShader("combineFragment", FRAGMENT);
    sm->LoadShaderSource("combineVertex"  , "shaders/gcombine.vert.glsl");
    sm->LoadShaderSource("combineFragment", "shaders/gcombine.frag.glsl");
    sm->CompileShader("combineVertex"  );
    sm->CompileShader("combineFragment");
    sm->AttachShaderToProgram("combine", "combineVertex"  );
    sm->AttachShaderToProgram("combine", "combineFragment");
    sm->LinkProgramObject("combine");
    std::cout << "combine program ID is " << (*sm)["combine"]->GetID() << std::endl;

    sm->CreateShaderProgram("null");
    sm->AttachShader("nullVertex"  , VERTEX  );
    sm->AttachShader("nullFragment", FRAGMENT);
    sm->LoadShaderSource("nullVertex"  , "shaders/null.vert.glsl");
    sm->LoadShaderSource("nullFragment", "shaders/null.frag.glsl");
    sm->CompileShader("nullVertex"  );
    sm->CompileShader("nullFragment");
    sm->AttachShaderToProgram("null", "nullVertex"  );
    sm->AttachShaderToProgram("null", "nullFragment");
    sm->LinkProgramObject("null");
    std::cout << "null program ID is " << (*sm)["null"]->GetID() << std::endl;

    // First pass stuffs

    // RGB : albedo
    glGenTextures(1, &colourTexture);
    glBindTexture(GL_TEXTURE_2D, colourTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // RG : View Space normal
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, windowWidth, windowHeight, 0, GL_RG, GL_FLOAT, 0);
    
    // R : Depth
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glGenFramebuffers(1, &gbufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, colourTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 , GL_TEXTURE_2D, normalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT  , GL_TEXTURE_2D, depthTexture , 0);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &specularTexture);
    glBindTexture(GL_TEXTURE_2D, specularTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &lightingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, emissiveTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, specularTexture, 0);
    
    GLenum lightingBuffers[2];
    lightingBuffers[0] = GL_COLOR_ATTACHMENT0;
    lightingBuffers[1] = GL_COLOR_ATTACHMENT1;
    glDrawBuffers(2, lightingBuffers);

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
    
    glGenBuffers(1, &cubeVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &cubeIndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    
    glEnable(GL_DEPTH_TEST);
    /*glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    */

    // Loading scene stuff

    
    ModelLoader modelLoader;
    
    SceneObject *vehicleObject = new SceneObject();
    modelLoader.loadSceneModel("models/R8.obj", vehicleObject);
    glm::mat4 vehicleModelMatrix = vehicleObject->getModelMatrix();
    vehicleModelMatrix = glm::scale(vehicleModelMatrix, glm::vec3(10, 10, 10));
    vehicleModelMatrix = glm::translate(vehicleModelMatrix, glm::vec3(5.0, 0.0, 0.0));
    vehicleObject->setModelMatrix(vehicleModelMatrix);
    sceneObjects.push_back(vehicleObject);
    
    SceneObject *sponzaObject = new SceneObject();
    modelLoader.loadSceneModel("models/sponza.obj", sponzaObject);
    sceneObjects.push_back(sponzaObject);
    
    std::cout << "<<<<< Model loading is done." << std::endl;
    
    // Creating lights
    for (int i = 0; i < 120; i++)
        sceneLights.push_back(new PointLight());

    glViewport(0, 0, (int) windowWidth, (int) windowHeight);

    gbufferProgramID  = (*sm)["gbuffer" ]->GetID();
    lightingProgramID = (*sm)["lighting"]->GetID();
    combineProgramID  = (*sm)["combine" ]->GetID();
    nullProgramID     = (*sm)["null"    ]->GetID();

    this->isInitialized = true;
}
