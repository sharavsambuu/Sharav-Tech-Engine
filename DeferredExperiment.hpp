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
    GLuint cubeVertexBufferID;
    GLuint cubeIndexBufferID;
    float cubeVertices[24] =
    {
         1.0f,  1.0f, -1.0f ,
        -1.0f,  1.0f, -1.0f ,
        -1.0f, -1.0f, -1.0f ,
         1.0f, -1.0f, -1.0f ,
         1.0f,  1.0f,  1.0f ,
        -1.0f,  1.0f,  1.0f ,
        -1.0f, -1.0f,  1.0f ,
         1.0f, -1.0f,  1.0f  
    };
    uint16_t cubeIndices[36]=
    {
        0, 2, 1,   0, 3, 2,
        4, 3, 0,   4, 7, 3,
        4, 1, 5,   4, 0, 1,
        1, 6, 5,   1, 2, 6,
        3, 6, 2,   3, 7, 6,
        5, 7, 4,   5, 6, 7,
    };

    ShaderManager* sm;

};


#endif	/* DEFERREDEXPERIMENT_HPP */

