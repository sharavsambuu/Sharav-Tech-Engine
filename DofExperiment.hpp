/* 
 * File:   DofExperiment.hpp
 * Author: prompto
 *
 * Created on December 13, 2013, 4:16 AM
 */

#ifndef DOFEXPERIMENT_HPP
#define	DOFEXPERIMENT_HPP

#include "AbstractState.hpp"

class DofExperiment : public AbstractState{
public:
    DofExperiment();
    ~DofExperiment();
    
    void pause();
    void resume();

    void input();
    void update();
    void render();
    
    void initialize();
private:
};

#endif	/* DOFEXPERIMENT_HPP */

