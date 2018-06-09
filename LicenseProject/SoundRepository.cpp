//
//  SoundRepository.cpp
//  LicenseProject
//
//  Created by Vlad Bonta on 09/06/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#include "SoundRepository.hpp"
#include <algorithm>
#include <numeric>
#include <future>
#include <unistd.h>

void searchAndReplace(std::string& value, std::string const& search,std::string const& replace){
    string::size_type  next;
    
    for(next = value.find(search); next != std::string::npos; next = value.find(search,next)) {
        value.replace(next,search.length(),replace);   
        next += replace.length();
    }
}

void playExercise(string exerciseString) {
    searchAndReplace(exerciseString, "-", "minus");
    searchAndReplace(exerciseString, "/", "impartit la");
    searchAndReplace(exerciseString, "*", "ori");
    playSound(exerciseString);
}

void playSound(string exercise) {
    string playString = "say -v Ioana " + exercise + "&";
    system(playString.c_str());
}


