#include "PointLight.hpp"
#include <glm/gtc/random.hpp>
#include <iostream>

PointLight::PointLight() {
    this->position = glm::vec3(0.0, 0.0, 0.0);
    this->color = glm::vec4(0.0, 0.0, 0.0, 1.0);
    this->radius = 5.0f;
    this->moveSpeed = 10.0f;
    this->maxBoundaries = glm::vec3(500.0, 500.0, 500.0);
    this->minBoundaries = -this->maxBoundaries;
    this->randomCheckPoint = glm::linearRand(this->minBoundaries, this->maxBoundaries);
}

PointLight::PointLight(glm::vec3 position, glm::vec4 color, float radius) {
    this->position = position;
    this->color = color;
    this->radius = radius;
}

PointLight::~PointLight() {
}

glm::vec3 PointLight::getPosition() {
    return this->position;
}

void PointLight::setPosition(glm::vec3 position) {
    this->position = position;
}

glm::vec4 PointLight::getColor() {
    return this->color;
}

void PointLight::setColor(glm::vec4 color) {
    this->color = color;
}

float PointLight::getRadius() {
    return this->radius;
}

void PointLight::setRadius(float radius) {
    this->radius = radius;
}

void PointLight::update(float dt) {
    glm::vec3 direction = this->randomCheckPoint - this->position;
    this->position += glm::normalize(direction) * this->moveSpeed * dt;
    std::cout << " check point " << "[" << randomCheckPoint.x << "," << randomCheckPoint.y << "," << randomCheckPoint.z << "]";
    std::cout << " light position " << "[" << position.x << "," << position.y << "," << position.z << "] " << std::endl;

    if (glm::length(direction) < 10.0f) {
        randomCheckPoint = glm::linearRand(this->minBoundaries, this->maxBoundaries);
        std::cout << "next random check point " << "[" << randomCheckPoint.x << "," << randomCheckPoint.y << "," << randomCheckPoint.z << "]" << std::endl;
    }
}

float PointLight::getSpeed() {
    return this->moveSpeed;
}
void PointLight::setSpeed(float speed) {
    this->moveSpeed = speed;
}

void PointLight::setMinBoundary(glm::vec3 p) {
    this->minBoundaries = p;
}

void PointLight::setMaxBoundary(glm::vec3 p) {
    this->maxBoundaries = p;
}