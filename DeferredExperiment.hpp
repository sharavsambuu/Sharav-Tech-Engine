/* 
 * File:   DeferredExperiment.hpp
 * Author: prompto
 *
 * Created on August 16, 2014, 11:05 PM
 */

#ifndef DEFERREDEXPERIMENT_HPP
#define	DEFERREDEXPERIMENT_HPP

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
#include "AbstractSceneObject.hpp"
#include "AbstractLight.hpp"
#include <vector>

class DeferredExperiment : public AbstractState {
public:
    DeferredExperiment();
    ~DeferredExperiment();

    void pause();
    void resume();

    void input();
    void update(float deltaTime);
    void render();

    void initialize();
    bool isInitializationDone();
private:
    std::vector<AbstractSceneObject *> sceneObjects;
    std::vector<AbstractLight       *> sceneLights;
    SceneObject *pointLightVolume;
    Camera* camera;
    
    bool isEntered;
    bool isFirstTime;
    float enteredTime;

    bool isInitialized;
    int windowWidth;
    int windowHeight;
    
    float zFarDistance;

    // logic
    bool wKeyPressed;
    bool sKeyPressed;
    bool aKeyPressed;
    bool dKeyPressed;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 inverseProjectionMatrix;

    // GPU shader programs
    GLuint gbufferProgramID;
    GLuint lightingProgramID;
    GLuint combineProgramID;
    GLuint nullProgramID;
    
    // first pass
    GLuint gbufferFBO;      // G-Buffer
    GLuint colourTexture;   // albedo
    GLuint normalTexture;   // normal
    GLuint depthTexture;    // depth
    
    // second pass
    GLuint lightingFBO;     // point lighting
    GLuint emissiveTexture; // emissive lighting information
    GLuint specularTexture; // specular lighting information

    GLuint quadBufferID;

    ShaderManager* sm;

};


#endif	/* DEFERREDEXPERIMENT_HPP */

