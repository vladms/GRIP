//
//  Square.hpp
//  LicenseProject
//
//  Created by Vlad Bonta on 17/03/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#ifndef Square_hpp
#define Square_hpp

#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;

class Square{
public:
    Square();
    Square(Point upperLeft);
    Point upperLeftCorner, lowerLeftCorner;
    void draw_rectangle(Mat src);
};

#endif /* Square_hpp */
