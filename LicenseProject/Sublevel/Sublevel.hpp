//
//  Sublevel.hpp
//  LicenseProject
//
//  Created by Vlad Bonta on 02/06/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#ifndef Sublevel_hpp
#define Sublevel_hpp

#include <stdio.h>
#include <string>
#include <queue>

using namespace std;

class Sublevel{
public:
    int level;
    int sublevelResponse;
    string sublevelExercise;
    vector<int> operands;
    vector<string> operators;
    
    Sublevel *clone();
};
#endif /* Sublevel_hpp */
