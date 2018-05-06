//
//  Square.cpp
//  LicenseProject
//
//  Created by Vlad Bonta on 17/03/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#include "Square.hpp"

#define BORDER_THICKNESS 2.0
#define SQUARE_SIZE 18.0

Scalar squareColor = Scalar(0, 255, 0);

Square::Square(){
    upperLeftCorner = Point(0,0);
    lowerLeftCorner = Point(0,0);
}


Square::Square(Point upperLeft) {
    upperLeftCorner = upperLeft;
    lowerLeftCorner = Point(upperLeftCorner.x + SQUARE_SIZE, upperLeftCorner.y + SQUARE_SIZE);
}

void Square::draw_rectangle(Mat src){
    rectangle(src,upperLeftCorner,lowerLeftCorner,squareColor,BORDER_THICKNESS);
}

float squareSize() {
    return SQUARE_SIZE;
}




