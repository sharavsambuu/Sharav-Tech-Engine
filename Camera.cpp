#include "Common.hpp"
#include "Camera.hpp"


Camera::Camera() {
    position = glm::vec3(0, 0, -3);
    horizontalAngle = 0.0f;
    verticalAngle = 0.0f;
    width = SCREEN_WIDTH;
    height = SCREEN_HEIGHT;
    moveSpeed = 100.0f;
    sensitivity = 0.001f;
    fov = 60.0f;
    zNear = 1.0;
    zFar = 10000.0f;
}

Camera::~Camera() {
}

void Camera::update(float deltaTime) {
    direction = glm::vec3(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
            );
    glm::vec3 right = glm::vec3(
            sin(horizontalAngle - glm::pi<float>() / 2.0f),
            0,
            cos(horizontalAngle - glm::pi<float>() / 2.0f)
            );
    up = glm::cross(right, direction);

    if (isMoveForward)
        position += direction * moveSpeed * deltaTime;
    if (isMoveBackward)
        position -= direction * moveSpeed * deltaTime;
    if (isMoveRight)
        position += right * moveSpeed * deltaTime;
    if (isMoveLeft)
        position -= right * moveSpeed * deltaTime;

    if (!height > 0)
        height = 1;
    projectionMatrix = glm::perspective(fov, width / height, zNear, zFar);
    viewMatrix = glm::lookAt(position, position + direction, up);

}

glm::mat4 Camera::getViewMatrix() {
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}

glm::mat4 Camera::getViewProjectionMatrix() {
    return projectionMatrix * viewMatrix;
}

glm::vec3 Camera::getPosition() {
    return position;
}

glm::vec3 Camera::getDirection() {
    return direction;
}

glm::vec3 Camera::getUp() {
    return up;
}

float Camera::getWidth() {
    return width;
}

float Camera::getHeight() {
    return height;
}

void Camera::updateAngles(float horizontal, float vertical) {
    horizontalAngle -= horizontal * sensitivity;
    verticalAngle -= vertical * sensitivity;
}

void Camera::updateHorizontalAngle(float horizontal) {
    horizontalAngle -= horizontal;
}

void Camera::updateVerticalAngle(float vertical) {
    verticalAngle -= vertical;
}

void Camera::setDimension(float height, float width) {
    this->height = height;
    this->width = width;
}

void Camera::updateFOV(float fov) {
    this->fov += fov / 100;
}

void Camera::moveForward(bool b) {
    isMoveForward = b;
}

void Camera::moveBackward(bool b) {
    isMoveBackward = b;
}

void Camera::moveLeft(bool b) {
    isMoveLeft = b;    
}

void Camera::moveRight(bool b) {
    isMoveRight = b;
}

float Camera::getFarDistance() {
    return this->zFar;
}