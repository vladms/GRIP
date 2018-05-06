//
//  Project.hpp
//  IP2017
//
//  Created by Vlad Bonta on 10/04/2017.
//  Copyright © 2017 Vlad Bonta. All rights reserved.
//

#ifndef Project_hpp
#define Project_hpp

#include <stdio.h>

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <math.h>
#include <queue>
#include <random>
#include <string>
#include <thread>

using namespace cv;
using namespace std;

#endif /* Project_hpp */





class SPoint{
public:
    int x, y;
    int superPixelId;
};

void startCamera();

