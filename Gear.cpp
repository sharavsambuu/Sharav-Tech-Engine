#include "Gear.hpp"
#include "GLFWApp.hpp"

Gear* Gear::instance = NULL;

Gear* Gear::getSingleton() {
    if (!Gear::instance)
        Gear::instance = new Gear();
    return Gear::instance;
}

Gear::Gear() {
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
}

bool Gear::keepRunning() {
    return GLFWApp::getSingleton()->keepRunning();
}

void Gear::update() {
    if (currentState)
        currentState->update();
}

void Gear::render() {
    if (currentState)
        currentState->render();
    GLFWApp::getSingleton()->updateVideo();
}

void Gear::input() {
    GLFWApp::getSingleton()->handleInput();
}

void Gear::addState(const std::string& name, AbstractState* state) {
    std::map<std::string, AbstractState*>::iterator it = states.find(name);
    if (it==states.end())
        states[name] = state;
}

void Gear::changeState(const std::string& name) {
    std::map<std::string, AbstractState*>::iterator it = states.find(name);
    if (it!=states.end()) {
        if (currentState)
            currentState->pause();
        currentState = states[name];
        currentState->resume();
    }
}

void Gear::exit() {
    isRunning = false;
}

