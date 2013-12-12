/* 
 * File:   Gear.hpp
 * Author: prompto
 *
 * Created on December 13, 2013, 12:27 AM
 */

#ifndef GEAR_HPP
#define	GEAR_HPP

#include "AbstractState.hpp"
#include <stdlib.h>
#include <string>
#include <map>

class Gear {
public:
    static Gear* getSingleton();
    Gear();
    ~Gear();
    bool keepRunning();
    void update();
    void render();
    void input();
    void addState(const std::string& name, AbstractState* state);
    void changeState(const std::string& name);
    void exit();
private:
    //static Gear
    bool isRunning;
    AbstractState* currentState;
    std::map<std::string, AbstractState*> states;
    
    static Gear* instance;
};

#endif	/* GEAR_HPP */

