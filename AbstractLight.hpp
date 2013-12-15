/* 
 * File:   AbstractLight.hpp
 * Author: prompto
 *
 * Created on December 15, 2013, 3:44 PM
 */

#ifndef ABSTRACTLIGHT_HPP
#define	ABSTRACTLIGHT_HPP

#include "glm/glm.hpp"

class AbstractLight {
public:
    AbstractLight();
    virtual ~AbstractLight();

    virtual void update(float dt) = 0;
    virtual glm::vec3 getPosition() = 0;
    virtual float getRadius() = 0;
    virtual glm::vec4 getColor() = 0;
    
};

#endif	/* ABSTRACTLIGHT_HPP */

