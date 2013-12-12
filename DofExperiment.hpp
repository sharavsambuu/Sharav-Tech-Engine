/* 
 * File:   DofExperiment.hpp
 * Author: prompto
 *
 * Created on December 13, 2013, 4:16 AM
 */

#ifndef DOFEXPERIMENT_HPP
#define	DOFEXPERIMENT_HPP

#include "AbstractState.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <GL/glew.h>
#include "Camera.hpp"
#include "ShaderManager.hpp"

class DofExperiment : public AbstractState {
public:
    DofExperiment();
    ~DofExperiment();

    void pause();
    void resume();

    void input();
    void update(float deltaTime);
    void render();

    void initialize();
private:
    bool isEntered;
    bool isFirstTime;
    float enteredTime;

    // logic
    bool wKeyPressed;
    bool sKeyPressed;
    bool aKeyPressed;
    bool dKeyPressed;

    bool doGaus;
    bool doDOF;

    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    GLuint bufferFBO;
    GLuint processFBO;
    GLuint colourTexture[2];
    GLuint depthTexture;

    Camera* camera;
    ShaderManager* sm;
};

#endif	/* DOFEXPERIMENT_HPP */

