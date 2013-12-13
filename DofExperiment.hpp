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
#include "SceneObject.hpp"
#include "ModelLoader.hpp"
#include <vector>

class DofExperiment : public AbstractState {
public:
    DofExperiment();
    ~DofExperiment();

    void pause();
    void resume();

    void input();
    void update(float deltaTime);
    void render();

    bool initialize();
    bool isInitializationDone();
private:
    bool isEntered;
    bool isFirstTime;
    float enteredTime;

    bool isInitialized;
    int windowWidth;
    int windowHeight;

    // logic
    bool wKeyPressed;
    bool sKeyPressed;
    bool aKeyPressed;
    bool dKeyPressed;

    bool doGaus;
    bool doDOF;

    glm::mat4 sponzaModelMatrix;
    glm::mat4 vehicleModelMatrix;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::mat4 mvpMatrix;
    glm::mat3 normalMatrix;

    GLuint bufferFBO;
    GLuint processFBO;
    GLuint colourTexture[2];
    GLuint depthTexture;

    Camera* camera;
    ShaderManager* sm;

    GLuint programID;
    GLuint quadProgramID;
    GLuint gausProgramID;
    GLuint dofProgramID;
    GLuint quadBufferID;

    std::vector<SceneObject *> sceneObjects;
    
    float animationTime;
    float rotationAngle;
    float ambientLightIntensity;
    glm::vec3 lightPosition;
};

#endif	/* DOFEXPERIMENT_HPP */

