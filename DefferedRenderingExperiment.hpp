/* 
 * File:   DefferedRenderingExperiment.hpp
 * Author: prompto
 *
 * Created on December 14, 2013, 7:48 AM
 */

#ifndef DEFFEREDRENDERINGEXPERIMENT_HPP
#define	DEFFEREDRENDERINGEXPERIMENT_HPP


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

class DefferedRenderingExperiment : public AbstractState {
public:
    DefferedRenderingExperiment();
    ~DefferedRenderingExperiment();

    void pause();
    void resume();

    void input();
    void update(float deltaTime);
    void render();

    void initialize();
    bool isInitializationDone();
private:
    std::vector<AbstractSceneObject *> sceneObjects;
    std::vector<AbstractLight *> sceneLights;
    SceneObject *pointLightVolume;
    Camera* camera;
    
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

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 mvpMatrix;
    glm::mat3 normalMatrix;

    GLuint gbufferProgramID;
    GLuint lightingProgramID;
    GLuint combineProgramID;
    GLuint nullProgramID;
    
    // first pass
    GLuint gbufferFBO; // G-Buffer
    GLuint colourTexture; // albedo
    GLuint normalTexture; // normal
    GLuint depthTexture; // depth
    // second pass
    GLuint lightingFBO; // point lighting
    GLuint emissiveTexture; // emissive lighting information
    GLuint specularTexture; // specular lighting information

    GLuint quadBufferID;

    ShaderManager* sm;

};

#endif	/* DEFFEREDRENDERINGEXPERIMENT_HPP */

