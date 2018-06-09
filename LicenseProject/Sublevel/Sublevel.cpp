//
//  Sublevel.cpp
//  LicenseProject
//
//  Created by Vlad Bonta on 02/06/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#include "Sublevel.hpp"


Sublevel * Sublevel::clone() {
    Sublevel *newSublevel = new Sublevel;
//    newSublevel->level = level;
    newSublevel->sublevelExercise = sublevelExercise;
    newSublevel->sublevelResponse = sublevelResponse;
    
    return newSublevel;
}
