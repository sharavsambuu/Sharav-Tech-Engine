/* 
 * File:   GLFWApp.hpp
 * Author: prompto
 *
 * Created on December 12, 2013, 10:24 PM
 */

#ifndef GLFWAPP_HPP
#define	GLFWAPP_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

class GLFWApp {
public:
    static GLFWApp* getSingleton();
    GLFWApp();
    ~GLFWApp();

    void handleInput();
    void updateVideo();
    bool keepRunning();

    bool getKeyPress(int key);
    bool getKeyRelease(int key);
    
    float getDeltaTime();
    int getFPS();

    void setCursorVisible(bool b);
    void setPinMouse(bool b);
    float getDeltaMouseX();
    float getDeltaMouseY();
    
    int getWindowWidth() { return this->windowWidth; }
    int getWindowHeight() { return this->windowHeight; }
private:
    bool isRunning;
    double startTime;
    double deltaTime;
    double buffTime;
    int framesPerSecond;
    int frameCounter;
    bool isPinMouse;
    int windowWidth;
    int windowHeight;
    float deltaMouseX;
    float deltaMouseY;
    GLFWwindow *window;

    static GLFWApp* instance;
};

#endif	/* GLFWAPP_HPP */

