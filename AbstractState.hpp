/* 
 * File:   AbstractState.hpp
 * Author: prompto
 *
 * Created on December 13, 2013, 12:24 AM
 */

#ifndef ABSTRACTSTATE_HPP
#define	ABSTRACTSTATE_HPP

class AbstractState {
public:
    AbstractState();
    virtual ~AbstractState() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void input() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    
    virtual bool isInitializationDone() = 0;

};

#endif	/* ABSTRACTSTATE_HPP */

