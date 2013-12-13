#include "DofExperiment.hpp"
#include "GLFWApp.hpp"
#include "Camera.hpp"
#include <iostream>
#include "ModelLoader.hpp"
#include "Common.hpp"
#include "Gear.hpp"
#include <exception>

DofExperiment::DofExperiment() {
    isEntered = true;
    isFirstTime = true;
    isInitialized = false;
}

DofExperiment::~DofExperiment() {
    std::cout << "<<<<< Dof experiment destruction function " << std::endl;
    for (auto object : sceneObjects) {
        delete object;
    }
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
        isInitialized = initialize();
        isFirstTime = false;
    }
}

void DofExperiment::input() {

}

void DofExperiment::update(float deltaTime) {

}

void DofExperiment::render() {

}

bool DofExperiment::initialize() {
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

    ModelLoader *modelLoader = new ModelLoader();
    SceneObject *object = new SceneObject();
    modelLoader->loadSceneModel("models/R8.obj", object);
    sceneObjects.push_back(object);
    return true;
}
