//
//  ImageRepository.hpp
//  LicenseProject
//
//  Created by Vlad Bonta on 28/05/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#ifndef ImageRepository_hpp
#define ImageRepository_hpp

#include <stdio.h>
#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


Mat loadMenuBackground();
Mat loadMenuButtonsBackground();

#endif /* ImageRepository_hpp */
