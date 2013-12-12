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
#include <vector>

class GLFWApp {
public:
    static GLFWApp* getSingleton();
    GLFWApp();
    ~GLFWApp();

    void handleInput();
    void updateVideo();
    bool keepRunning();

    float getDeltaTime();
    int getFPS();

    void setCursorVisible(bool b);
    void setPinMouse(bool b);
    int getDeltaMouseX();
    int getDeltaMouseY();
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
    int midWidth;
    int midHeight;
    int deltaMouseX;
    int deltaMouseY;
    GLFWwindow *window;

    static GLFWApp* instance;
};

#endif	/* GLFWAPP_HPP */
