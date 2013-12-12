#include "GLFWApp.hpp"
#include "Common.hpp"
#include <iostream>

GLFWApp* GLFWApp::instance = NULL;

GLFWApp* GLFWApp::getSingleton() {
    if (!GLFWApp::instance)
        GLFWApp::instance = new GLFWApp();
    return GLFWApp::instance;
}

GLFWApp::GLFWApp() {
    this->windowWidth = (int) SCREEN_WIDTH;
    this->windowHeight = (int) SCREEN_HEIGHT;
    this->framesPerSecond = 0;
    this->frameCounter = 0;
    this->startTime = glfwGetTime();
    this->deltaTime = 0.001f;
    this->buffTime = 0.0f;
    this->isPinMouse = true;
    this->isRunning = true;

    if (!glfwInit()) {
        std::cout << "Error: GLFW failed to initialize.\n";
        return;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "STE", NULL, NULL);
    if (!window) {
        std::cout << "Error: Failed to create window.\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        std::cout << "Error: Failed to load OpenGL functions.\n";
        glfwTerminate();
        return;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

GLFWApp::~GLFWApp() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLFWApp::handleInput() {
    isRunning = !glfwWindowShouldClose(window) || 
            getKeyPress(GLFW_KEY_ESCAPE);
    if (isPinMouse) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        deltaMouseX = mouseX - windowWidth / 2;
        deltaMouseY = mouseY - windowHeight / 2;
        glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
    }
}

void GLFWApp::updateVideo() {
    glfwSwapBuffers(window);
    glfwPollEvents();
    float lastTime = glfwGetTime();
    deltaTime = lastTime - startTime;
    startTime = lastTime;
    frameCounter++;
    buffTime += deltaTime;
    if (buffTime > 1.0f) {
        buffTime = 0.0f;
        framesPerSecond = frameCounter;
        frameCounter = 0;
    }
}

bool GLFWApp::keepRunning() {
    return this->isRunning;
}

float GLFWApp::getDeltaTime() {
    return this->deltaTime;
}

int GLFWApp::getFPS() {
    return this->framesPerSecond;
}

void GLFWApp::setCursorVisible(bool b) {
    if (!b) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool GLFWApp::getKeyPress(int key) {
    return glfwGetKey(window, key)==GLFW_PRESS;
}

bool GLFWApp::getKeyRelease(int key) {
    return glfwGetKey(window, key)==GLFW_RELEASE;
}

void GLFWApp::setPinMouse(bool b) {
    this->isPinMouse = b;
}

int GLFWApp::getDeltaMouseX() {
    return this->deltaMouseX;
}

int GLFWApp::getDeltaMouseY() {
    return this->deltaMouseY;
}










