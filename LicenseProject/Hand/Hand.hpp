//
//  Hand.hpp
//  LicenseProject
//
//  Created by Vlad Bonta on 04/06/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#ifndef Hand_hpp
#define Hand_hpp

#include <stdio.h>
#include <string>
#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "Square.hpp"
using namespace std;
using namespace cv;

class Hand{
public:
    Rect handROI;
    Mat handImage;
    Mat handImageBinarized;
    Rect handDetectedRect;
    Rect originalHandDetectedRect;
    Mat handImageDT;
    Mat handImageFilledContours;
    Point handDetectedCenterPoint;
    vector<Point> handBiggestContour;
    vector<Point> detectedFingersPositions;
    Point topMostHandFinger;
    vector<Point> handHullPoints;
    int nrOfFingers;
    array<int, 6> handReadNumbers;
    bool multipleContinuousFingersDetected;
    vector <Square> handSquarePoints;

};
#endif /* Hand_hpp */
