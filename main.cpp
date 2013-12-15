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
#include "DefferedRenderingExperiment.hpp"
using namespace std;

/*
 * "SHARAV TECH ENGINE" experiments
 */
int main(int argc, char** argv) {
    std::cout << "<<<<< WELCOME TO STE 0.0.1 >>>>>" << std::endl;
    std::cout << "<<<<< SYSTEM STARTING >>>>>" << std::endl;
    Gear *gear = Gear::getSingleton();
    DofExperiment *dofExp = new DofExperiment();
    DefferedRenderingExperiment *defferedRenderingExp = new DefferedRenderingExperiment();
    gear->addState("dof", dofExp);
    gear->addState("deffered", defferedRenderingExp);
    gear->changeState("deffered");
    while (gear->keepRunning()) {
        gear->input();
        gear->update();
        gear->render();
    }
    delete gear;
    std::cout << "<<<<< SYSTEM ENDED >>>>>" << std::endl;
    return 0;
}