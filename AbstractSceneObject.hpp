/* 
 * File:   AbstractSceneObject.hpp
 * Author: prompto
 *
 * Created on December 14, 2013, 5:43 AM
 */

#ifndef ABSTRACTSCENEOBJECT_HPP
#define	ABSTRACTSCENEOBJECT_HPP

#include <GL/glew.h>

class AbstractSceneObject {
public:
    AbstractSceneObject();
    virtual ~AbstractSceneObject();

    virtual void update(float dt) = 0;
    virtual void render(GLuint programID) = 0;
};

#endif	/* ABSTRACTSCENEOBJECT_HPP */

