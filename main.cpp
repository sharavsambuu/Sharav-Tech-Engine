/* 
 * File:   main.cpp
 * Author: prompto
 *
 * Created on November 30, 2013, 4:48 PM
 */

#include <cstdlib>
#include <iostream>
#include "GLFWApp.hpp"
#include "Gear.hpp"
#include "DofExperiment.hpp"
#include "DeferredExperiment.hpp"
using namespace std;

/*
 * "SHARAV TECH ENGINE" experiments
 */
int main(int argc, char** argv) {
    std::cout << "<<<<< WELCOME TO STE 0.0.1 >>>>>" << std::endl;
    std::cout << "<<<<< SYSTEM STARTING >>>>>" << std::endl;
    Gear *gear = Gear::getSingleton();
    DofExperiment *dofExp = new DofExperiment();
    DeferredExperiment *deferredExp = new DeferredExperiment();
    gear->addState("dof"     , dofExp);
    gear->addState("deferred", deferredExp);
    gear->changeState("deferred");
    while (gear->keepRunning()) {
        gear->input();
        gear->update();
        gear->render();
    }
    delete gear;
    std::cout << "<<<<< SYSTEM ENDED >>>>>" << std::endl;
    return 0;
}