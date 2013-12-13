#include "Gear.hpp"
#include "GLFWApp.hpp"
#include <iostream>

Gear* Gear::instance = NULL;

Gear* Gear::getSingleton() {
    if (!Gear::instance)
        Gear::instance = new Gear();
    return Gear::instance;
}

Gear::Gear() {
    std::cout << "<<<<< Starting GEAR system " << std::endl;
    GLFWApp::getSingleton();
    currentState = NULL;
    isRunning = true;
}

Gear::~Gear() {
    for (auto& state : states) {
        delete state.second;
    }
    states.clear();
    delete GLFWApp::getSingleton();
    std::cout << "<<<<< ending GEAR system " << std::endl;
}

bool Gear::keepRunning() {
    return GLFWApp::getSingleton()->keepRunning() && isRunning;
}

void Gear::update() {
    if (currentState) {
        if (currentState->isInitializationDone()==true)
            currentState->update(GLFWApp::getSingleton()->getDeltaTime());
    }
}

void Gear::render() {
    if (currentState)
        if (currentState->isInitializationDone()==true)
            currentState->render();
    GLFWApp::getSingleton()->updateVideo();
}

void Gear::input() {
    GLFWApp::getSingleton()->handleInput();
    if (currentState)
        if (currentState->isInitializationDone()==true)
            currentState->input();
}

void Gear::addState(const std::string& name, AbstractState* state) {
    std::map<std::string, AbstractState*>::iterator it = states.find(name);
    if (it == states.end())
        states[name] = state;
}

void Gear::changeState(const std::string& name) {
    std::map<std::string, AbstractState*>::iterator it = states.find(name);
    if (it != states.end()) {
        if (currentState)
            currentState->pause();
        currentState = states[name];
        currentState->resume();
    }
}

void Gear::exit() {
    isRunning = false;
}

