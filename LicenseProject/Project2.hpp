//
//  Project2.hpp
//  LicenseProject
//
//  Created by Vlad Bonta on 15/05/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#ifndef Project2_hpp
#define Project2_hpp

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
#include "Level.hpp"
#include "SoundRepository.hpp"

using namespace cv;
using namespace std;
void updatePlayingLevel(Level *currentLevel);
void setupLevel(Level *level);
void startCamera(VideoCapture cap);
int newFrame(VideoCapture cap, Mat frame, int state, bool updateResult, int secondsLeft);

#endif /* Project_hpp */
