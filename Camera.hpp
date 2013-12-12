/* 
 * File:   Camera.hpp
 * Author: prompto
 *
 * Created on December 13, 2013, 1:33 AM
 */

#ifndef CAMERA_HPP
#define	CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <math.h>

class Camera {
public:
    Camera();
    ~Camera();

    void update(float deltaTime);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewProjectionMatrix();

    glm::vec3 getPosition();
    glm::vec3 getDirection();
    glm::vec3 getUp();

    float getWidth();
    float getHeight();

    void updateAngles(float horizontal, float vertical);
    void updateHorizontalAngle(float horizontal);
    void updateVerticalAngle(float vertical);
    void setDimension(float height, float width);
    void updateFOV(float fov);

    void moveForward(bool b);
    void moveBackward(bool b);
    void moveLeft(bool b);
    void moveRight(bool b);
protected:
private:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;

    float horizontalAngle;
    float verticalAngle;

    float moveSpeed;
    float sensitivity;

    float fov;
    float zNear;
    float zFar;

    float width;
    float height;

    bool isMoveForward;
    bool isMoveBackward;
    bool isMoveLeft;
    bool isMoveRight;
};


#endif	/* CAMERA_HPP */

