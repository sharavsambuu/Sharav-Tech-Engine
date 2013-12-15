#include "PointLight.hpp"

PointLight::PointLight() {
    this->position = glm::vec3(0.0, 0.0, 0.0);
    this->color = glm::vec4(0.0, 0.0, 0.0, 1.0);
    this->radius = 5.0f;
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
    
}