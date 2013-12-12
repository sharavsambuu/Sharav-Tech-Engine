#include "DofExperiment.hpp"
#include "GLFWApp.hpp"
#include <iostream>

DofExperiment::DofExperiment() {
    isEntered = true;
    isFirstTime = true;
}

DofExperiment::~DofExperiment() {
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
    //aKeyPressed = app->getKeyPress(GLFW_KEY_A);
    //sKeyPressed = app->getKeyPress(GLFW_KEY_S);
    //dKeyPressed = app->getKeyPress(GLFW_KEY_D);
}

void DofExperiment::update(float deltaTime) {
    //    std::cout << " DoF exp is running " << deltaTime << std::endl;
    if (wKeyPressed)
        std::cout << "moving forward" << std::endl;
}

void DofExperiment::render() {
}

void DofExperiment::initialize() {
    std::cout << "<<<<< DoF initialization and running" << std::endl;
    wKeyPressed = false;
    aKeyPressed = false;
    sKeyPressed = false;
    dKeyPressed = false;
}
