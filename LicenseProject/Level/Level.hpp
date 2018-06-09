//
//  Level.hpp
//  LicenseProject
//
//  Created by Vlad Bonta on 02/06/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#ifndef Level_hpp
#define Level_hpp

#include <stdio.h>
#include <string>
#include "Sublevel.hpp"
#include <array>
#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Level{
public:
    Level();
    int level;
    int seconds;
    int numberOfSublevels;
    int currentSublevelIndex;
    int score;
    Mat levelImage;
    array<Sublevel *, 5> sublevels;
    void initLevel(int levelNumber);
};

#endif /* Level_hpp */
