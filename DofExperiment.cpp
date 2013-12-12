#include "DofExperiment.hpp"
#include "GLFWApp.hpp"
#include "Camera.hpp"
#include <iostream>

DofExperiment::DofExperiment() {
    isEntered = true;
    isFirstTime = true;
}

DofExperiment::~DofExperiment() {
    std::cout << "<<<<< Dof experiment destruction function " << std::endl;
    if (sm != NULL)
        delete sm;
}

void DofExperiment::pause() {
    isEntered = false;
}

void DofExperiment::resume() {
    isEntered = true;
    enteredTime = 0.0f;
    std::cout << "<<<<< Entered DoF experimentation State" << std::endl;
    if (isFirstTime) {
        initialize();
        isFirstTime = false;
    }
}

void DofExperiment::input() {
    GLFWApp *app = GLFWApp::getSingleton();
    if (app->getKeyPress(GLFW_KEY_W))
        wKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_W))
        wKeyPressed = false;
    if (app->getKeyPress(GLFW_KEY_A))
        aKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_A))
        aKeyPressed = false;
    if (app->getKeyPress(GLFW_KEY_S))
        sKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_S))
        sKeyPressed = false;
    if (app->getKeyPress(GLFW_KEY_D))
        dKeyPressed = true;
    if (app->getKeyRelease(GLFW_KEY_D))
        dKeyPressed = false;
    camera->updateAngles(app->getDeltaMouseX(), app->getDeltaMouseY());
    camera->moveForward(wKeyPressed);
    camera->moveBackward(sKeyPressed);
    camera->moveLeft(aKeyPressed);
    camera->moveRight(dKeyPressed);
}

void DofExperiment::update(float deltaTime) {
    if (wKeyPressed)
        std::cout << "moving forward" << std::endl;
    if (aKeyPressed)
        std::cout << "moving left" << std::endl;
    if (sKeyPressed)
        std::cout << "moving backward" << std::endl;
    if (dKeyPressed)
        std::cout << "moving right" << std::endl;
    camera->update(deltaTime);
}

void DofExperiment::render() {
}

void DofExperiment::initialize() {
    std::cout << "<<<<< DoF initialization and running" << std::endl;
    wKeyPressed = false;
    aKeyPressed = false;
    sKeyPressed = false;
    dKeyPressed = false;

    doGaus = false;
    doDOF = true;
    camera = new Camera();

    // SHADERS
    sm = ShaderManager::getSingleton();

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

}
