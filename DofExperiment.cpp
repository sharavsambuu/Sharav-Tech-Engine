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
    
    return true;
}
