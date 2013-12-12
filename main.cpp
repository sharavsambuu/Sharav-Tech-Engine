/* 
 * File:   main.cpp
 * Author: prompto
 *
 * Created on November 30, 2013, 4:48 PM
 */

#include <cstdlib>
#include <iostream>
#include "Gear.hpp"
#include "DofExperiment.hpp"
using namespace std;

/*
 * "SHARAV TECH ENGINE" experiments
 */
int main(int argc, char** argv) {
    std::cout << "<<<<< WELCOME TO STE 0.01 >>>>>" << std::endl;
    std::cout << "<<<<< SYSTEM STARTING >>>>>" << std::endl;

    Gear *gear = Gear::getSingleton();
    gear->addState("dof", new DofExperiment());
    gear->changeState("dof");

    while (gear->keepRunning()) {
        gear->input();
        gear->update();
        gear->render();
    }
    
    delete gear;
    std::cout << "<<<<< SYSTEM ENDED >>>>>" << std::endl;
    return 0;
}