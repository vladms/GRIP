//
//  Level.cpp
//  LicenseProject
//
//  Created by Vlad Bonta on 02/06/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#include "Level.hpp"
#include "ImageRepository.hpp"

Level::Level(){
    seconds = 5;
    level = 0;
    numberOfSublevels = 4;
    currentSublevelIndex = 0;
}
void Level::initLevel(int levelNumber) {
    level = levelNumber;
    currentSublevelIndex = 0;
    switch (levelNumber) {
        case 1: {
            levelImage = loadAppleImage();
            vector<int> operands;
            vector<string> operators;
            operands.push_back(1);
            operands.push_back(1);
            operators.push_back("+");
            
            Sublevel *sublevel = new Sublevel;
            sublevel->sublevelResponse = 2;
            sublevel->sublevelExercise = "1 + 1";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(0) = sublevel;
            
            operands.clear();
            operands.push_back(1);
            operands.push_back(3);
            operators.clear();
            operators.push_back("+");
            
            sublevel = sublevel->clone();
            sublevel->sublevelResponse = 4;
            sublevel->sublevelExercise = "1 + 3";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(1) = sublevel;
            
            operands.clear();
            operands.push_back(2);
            operands.push_back(3);
            operators.clear();
            operators.push_back("+");
            
            
            sublevel = sublevel->clone();
            sublevel->sublevelResponse = 5;
            sublevel->sublevelExercise = "2 + 3";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(2) = sublevel;
            
            
            operands.clear();
            operands.push_back(3);
            operands.push_back(4);
            operators.clear();
            operators.push_back("+");
            
            sublevel = sublevel->clone();
            sublevel->sublevelResponse = 7;
            sublevel->sublevelExercise = "3 + 4";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(3) = sublevel;
            
            operands.clear();
            operands.push_back(3);
            operands.push_back(3);
            operators.clear();
            operators.push_back("+");
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "3 + 3";
            sublevel->sublevelResponse = 6;
            
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(4) = sublevel;
            
            break;
        }
        case 2: {
            levelImage = loadLemonImage();
            vector<int> operands;
            vector<string> operators;
            operands.push_back(2);
            operands.push_back(1);
            operators.push_back("-");
            
            Sublevel *sublevel = new Sublevel;
            sublevel->sublevelResponse = 1;
            sublevel->sublevelExercise = "2 - 1";
            sublevel->operands = operands;
            sublevel->operators = operators;
            sublevels.at(0) = sublevel;
            
            operands.clear();
            operands.push_back(4);
            operands.push_back(1);
            operators.clear();
            operators.push_back("-");
            
            sublevel = sublevel->clone();
            sublevel->sublevelResponse = 3;
            sublevel->sublevelExercise = "4 / 1";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(1) = sublevel;
            
            operands.clear();
            operands.push_back(11);
            operands.push_back(5);
            operators.clear();
            operators.push_back("-");
            
            
            sublevel = sublevel->clone();
            sublevel->sublevelResponse = 6;
            sublevel->sublevelExercise = "11 * 5";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(2) = sublevel;
            
            
            operands.clear();
            operands.push_back(9);
            operands.push_back(1);
            operators.clear();
            operators.push_back("-");
            
            sublevel = sublevel->clone();
            sublevel->sublevelResponse = 8;
            sublevel->sublevelExercise = "9 - 1";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(3) = sublevel;
            
            operands.clear();
            operands.push_back(5);
            operands.push_back(2);
            operators.clear();
            operators.push_back("-");
            
            sublevel = sublevel->clone();
            sublevel->sublevelResponse = 3;
            sublevel->sublevelExercise = "5 - 2";
            sublevel->operands = operands;
            sublevel->operators = operators;
            
            sublevels.at(4) = sublevel;
            break;
        }
        case 3: {
            Sublevel *sublevel = new Sublevel;
            sublevel->sublevelExercise = "2 + 0";
            sublevel->sublevelResponse = 2;
            sublevels.at(0) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "2 + 1";
            sublevel->sublevelResponse = 3;
            
            sublevels.at(1) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "2 + 2";
            sublevel->sublevelResponse = 4;
            
            sublevels.at(2) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "2 + 3";
            sublevel->sublevelResponse = 5;
            
            sublevels.at(3) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "2 + 4";
            sublevel->sublevelResponse = 6;
            
            sublevels.at(4) = sublevel;
            
            break;
        }
        case 4: {
            Sublevel *sublevel = new Sublevel;
            sublevel->sublevelExercise = "2 - 0";
            sublevel->sublevelResponse = 2;
            sublevels.at(0) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "2 - 1";
            sublevel->sublevelResponse = 1;
            
            sublevels.at(1) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "2 - 2";
            sublevel->sublevelResponse = 0;
            
            sublevels.at(2) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "5 - 3";
            sublevel->sublevelResponse = 2;
            
            sublevels.at(3) = sublevel;
            
            sublevel = sublevel->clone();
            sublevel->sublevelExercise = "8 - 4";
            sublevel->sublevelResponse = 4;
            
            sublevels.at(4) = sublevel;
            
            break;
        }
        default:
            break;
    }
}
