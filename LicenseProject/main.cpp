// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdio.h>
#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <math.h>
#include <queue>
#include <random>
#include "Project2.hpp"
#include <string>
#include "dibfft.h"

using namespace cv;
using namespace std;

void showLevelsMenu();
void setupStartMenu();

struct point{
    int i,j;
};

int menuState;
Mat3b canvas;
string startButtonText("Start learning!");
string menuWindowName = "Learning is COOL";
string levelsWindowName = "Choose your level";

Rect startButton;
Rect backButton;
Rect level1Button;
Rect level2Button;

void callBackFunc(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        cout<< x<< " " << y << endl;
        if (backButton.contains(Point(x, y)) && menuState == 1) {
            setupStartMenu();
            cout << "Clicked back button!" << endl;
        }
        if (startButton.contains(Point(x, y)) && menuState == 0) {
            showLevelsMenu();
            cout << "Clicked start!" << endl;
            
            //            rectangle(canvas(button), button, Scalar(0,0,255), 2);
        }
        
        
        if (level1Button.contains(Point(x, y))) {
            cout << "Clicked level1!" << endl;
        }
        if (level2Button.contains(Point(x, y))) {
            
            cout << "Clicked level2!" << endl;
        }
        
        
        
    }
    if (event == EVENT_LBUTTONUP) {
        //        rectangle(canvas, startButton, Scalar(200, 200, 200), 2);
    }
    
    //    imshow(menuWindowName, canvas);
    waitKey(1);
}

void showLevelsMenu() {
    menuState = 1;
    Mat3b img(600, 600, Vec3b(0, 255, 0));
    
    backButton = Rect(img.cols / 4,0,img.cols / 2, 50);
    level1Button = Rect(0,60,img.cols, 50);
    level2Button = Rect(0,120,img.cols, 50);
    
    // The canvas
    canvas = Mat3b(img.rows + 3 * backButton.height, img.cols, Vec3b(255.0, 255.0, 255.0));
    
    // Draw the button
    canvas(backButton) = Vec3b(200,200,200);
    canvas(level1Button) = Vec3b(200,200,200);
    canvas(level2Button) = Vec3b(200,200,200);
    
    putText(canvas(backButton), "Back", Point(backButton.width / 2.0, backButton.height*0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
    putText(canvas(level1Button), "Level 1", Point(level1Button.width / 2.0, level1Button.height*0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
    putText(canvas(level2Button), "Level 2", Point(level2Button.width / 2.0, level2Button.height*0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
    
    // Setup callback function
    //    namedWindow(menuWindowName);
    //    setMouseCallback(menuWindowName, callBackFunc);
    
    imshow(menuWindowName, canvas);
    waitKey();
}

void setupStartMenu() {
    menuState = 0;
    // An image
    Mat3b img(600, 600, Vec3b(0, 255, 0));
    
    // Your button
    startButton = Rect(0,0,img.cols, 50);
    
    // The canvas
    canvas = Mat3b(img.rows + startButton.height, img.cols, Vec3b(255.0, 255.0, 255.0));
    
    // Draw the button
    canvas(startButton) = Vec3b(200,200,200);
    putText(canvas(startButton), startButtonText, Point(startButton.width*0.35, startButton.height*0.7), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
    
    // Draw the image
    //    img.copyTo(canvas(Rect(0, startButton.height, img.cols, img.rows)));
    
    // Setup callback function
    namedWindow(menuWindowName);
    setMouseCallback(menuWindowName, callBackFunc);
    
    imshow(menuWindowName, canvas);
    waitKey();
}



//void displayHistogramArray(int *histogram, String windowName){
//    int maximaOfHistogram = 0;
//    
//    for (int i = 0; i <= 255; ++i){
//        if (histogram[i] > maximaOfHistogram){
//            maximaOfHistogram = histogram[i];
//        }
//    }
//    
//    Mat histogramImage = Mat(255, 255, CV_8UC3, CV_RGB(0, 0, 0));
//    for (int i = 0; i <= 255; ++i){
//        float aux = (float)((float)histogram[i] / maximaOfHistogram);
//        histogram[i] = (int)(aux * 255.0);
//    }
//    for (int i = 0; i <= 255; ++i){
//        Point p1 = Point(i, 255);
//        Point p2 = Point(i, 255 - histogram[i]);
//        
//        line(histogramImage, p1, p2, CV_RGB(255, 0, 0));
//    }
//    imshow(windowName, histogramImage);
//}

int main(){
    
    //    system("afplay /Users/VladBonta/Desktop/test.mp3");
    int op;
    //    do
    //    {
    //        system("cls");
    //        destroyAllWindows();
    //        printf("Menu:\n");
    //        printf(" 1 - Open laptop camera\n");
    //
    //
    //
    //        printf(" 0 - Exit\n\n");
    //        printf("Option: ");
    //        scanf("%d",&op);
    //        switch (op) {
    //            case 1:
    //                startCamera();
    //                break;
    //            default:
    //                exit(0);
    //        }
    //
    //    }
    //    while (op!=0);
//        setupStartMenu();
        startCamera();
//    findHand();
    
    
    return 0;
}


