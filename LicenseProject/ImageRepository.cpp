//
//  ImageRepository.cpp
//  LicenseProject
//
//  Created by Vlad Bonta on 28/05/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#include "ImageRepository.hpp"

Mat loadMenuBackground() {
    Mat image;
    image = imread("LicenseProject/Images/ButtonBackgroundImage.jpg", CV_LOAD_IMAGE_COLOR);
    
    if(!image.data) {
        cout <<  "Could not open or find the image" << std::endl ;
        return Mat();
    }
    return image;
    
}
Mat loadMenuButtonsBackground() {
    Mat image;
    image = imread("LicenseProject/Images/BlueButtonBackground.png", CV_LOAD_IMAGE_COLOR);
    
    if(!image.data) {
        cout <<  "Could not open or find the image" << std::endl ;
        return Mat();
    }
    return image;
    }

