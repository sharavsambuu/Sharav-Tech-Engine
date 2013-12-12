/* 
 * File:   DofExperiment.hpp
 * Author: prompto
 *
 * Created on December 13, 2013, 4:16 AM
 */

#ifndef DOFEXPERIMENT_HPP
#define	DOFEXPERIMENT_HPP

#include "AbstractState.hpp"

class DofExperiment : public AbstractState {
public:
    DofExperiment();
    ~DofExperiment();

    void pause();
    void resume();

    void input();
    void update(float deltaTime);
    void render();

    void initialize();
private:
    bool isEntered;
    bool isFirstTime;
    float enteredTime;

    // logic
    bool wKeyPressed;
    bool sKeyPressed;
    bool aKeyPressed;
    bool dKeyPressed;

};

#endif	/* DOFEXPERIMENT_HPP */

