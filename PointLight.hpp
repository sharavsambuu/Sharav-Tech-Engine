/* 
 * File:   PointLight.hpp
 * Author: prompto
 *
 * Created on December 15, 2013, 4:17 PM
 */

#ifndef POINTLIGHT_HPP
#define	POINTLIGHT_HPP

#include "AbstractLight.hpp"
#include <glm/glm.hpp>

class PointLight : public AbstractLight {
public:
    PointLight();
    PointLight(glm::vec3 position, glm::vec4 color, float radius);
    ~PointLight();
    
    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);
    
    glm::vec4 getColor();
    void setColor(glm::vec4 color);
    
    float getRadius();
    void setRadius(float radius);
    
    float getSpeed();
    void setSpeed(float speed);
    
    void setMinBoundary(glm::vec3 p);
    void setMaxBoundary(glm::vec3 p);
    
    void update(float dt);
    
private:
    
    glm::vec3 position;
    glm::vec4 color;
    float radius;
    float moveSpeed;
    glm::vec3 randomCheckPoint;
    glm::vec3 minBoundaries;
    glm::vec3 maxBoundaries;
};

#endif	/* POINTLIGHT_HPP */

