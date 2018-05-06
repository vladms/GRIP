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
#include "Project.hpp"
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



void displayHistogramArray(int *histogram, String windowName){
    int maximaOfHistogram = 0;
    
    for (int i = 0; i <= 255; ++i){
        if (histogram[i] > maximaOfHistogram){
            maximaOfHistogram = histogram[i];
        }
    }
    
    Mat histogramImage = Mat(255, 255, CV_8UC3, CV_RGB(0, 0, 0));
    for (int i = 0; i <= 255; ++i){
        float aux = (float)((float)histogram[i] / maximaOfHistogram);
        histogram[i] = (int)(aux * 255.0);
    }
    for (int i = 0; i <= 255; ++i){
        Point p1 = Point(i, 255);
        Point p2 = Point(i, 255 - histogram[i]);
        
        line(histogramImage, p1, p2, CV_RGB(255, 0, 0));
    }
    imshow(windowName, histogramImage);
}

void findHand() {
    // Load the image
    Mat src = imread("/Users/VladBonta/Mine/License/LicenseProject/LicenseProject/hand.png");
    // Check if everything was fine
    if (!src.data)
        return ;
    // Show source image
    imshow("Source Image", src);
    // Create a kernel that we will use for accuting/sharpening our image
    Mat kernel = (Mat_<float>(3,3) <<
                  1,  1, 1,
                  1, -8, 1,
                  1,  1, 1); // an approximation of second derivative, a quite strong kernel
    // do the laplacian filtering as it is
    // well, we need to convert everything in something more deeper then CV_8U
    // because the kernel has some negative values,
    // and we can expect in general to have a Laplacian image with negative values
    // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
    // so the possible negative number will be truncated
    Mat imgLaplacian;
    Mat sharp = src; // copy source image to another temporary one
    filter2D(sharp, imgLaplacian, CV_32F, kernel);
    src.convertTo(sharp, CV_32F);
    Mat imgResult = sharp - imgLaplacian;
    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    // imshow( "Laplace Filtered Image", imgLaplacian );
    imshow( "New Sharped Image", imgResult );
    src = imgResult; // copy back
    // Create binary image from source image
    Mat bw;
    cvtColor(src, bw, CV_BGR2GRAY);
    threshold(bw, bw, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    imshow("Binary Image", bw);
    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(bw, dist, CV_DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1., NORM_MINMAX);
    imshow("Distance Transform Image", dist);
    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    //    threshold(dist, dist,127,255,CV_THRESH_BINARY);
    
    threshold(dist, dist, .8, 1., CV_THRESH_BINARY);
    //    cvtColor(dist,dist,CV_RGB2GRAY);
    
    //    Mat distBW;
    
    //    dist.convertTo(distBW, CV_8U);
    //
    //    Mat nonZeroCoordinates;
    //
    //    findNonZero(distBW, nonZeroCoordinates);
    //    for (int i = 0; i < nonZeroCoordinates.total(); i++ ) {
    //        cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << nonZeroCoordinates.at<Point>(i).y << endl;
    //    }
    Point palmCenter;
    float bestColour = -1;
    int numberOfMaxPoints = 0;
    for (int i = 0; i < dist.rows; ++i) {
        for (int j = 0; j < dist.cols; ++j) {
            float pos = dist.at<float>(Point(i, j));
            
            if (pos > 0) {
                //                cout<< "Colour: " << pos.val[0] << " " << pos.val[1] << " " << pos.val[2] << "\n";
                //                              cout<< "Colour: " << pos << "\n";
            }
            //            if (pos.val[0] + pos.val[1] + pos.val[2] > bestColour.val[0] + bestColour.val[1] + bestColour.val[2]) {
            //            if (pos > bestColour) {
            //                palmCenter = Point(i, j);
            //                bestColour = pos;
            //            }
            if (pos == 1) {
                palmCenter.x += i;
                palmCenter.y += j;
                numberOfMaxPoints++;
                //                bestColour = pos;
            }
            
        }
    }
    palmCenter.x /= numberOfMaxPoints;
    palmCenter.y /= numberOfMaxPoints;
    
    cout<< "Palm center: " << palmCenter.x << " " << palmCenter.y << " " << dist.channels() <<"\n";
    
    //Inner circle of the maximal radius
    
    float innerCircleRadius = 80;
    
    
    
    Mat innercircleImage;
    Mat testingSrc = src.clone();
    float outerCircleRadius = innerCircleRadius + 60.0/100.0 * innerCircleRadius;
    circle(testingSrc, palmCenter, innerCircleRadius, Scalar(255,0, 125));
    circle(testingSrc, palmCenter, outerCircleRadius, Scalar(100,0, 125));
    imshow("Palm center", testingSrc);
    
    Mat filledPalmMask = Mat(src.rows, src.cols, CV_32F);
    circle(filledPalmMask, palmCenter, innerCircleRadius, Scalar(255,255, 255), -1);
    
    imshow("filledPalmMask ",filledPalmMask);
    Mat palmMask = src.clone();
    
    float X = 0;
    float Y = 0;
    int radius = innerCircleRadius;
    radius = 1;
    vector<Point> points;
    imshow("src before ",src);
    
    
    //    Point points[100000];
    int nrOfPoints = 0;
    //    for (int t = 0; t <= 360 && radius < src.cols; ++t,radius++) {
    for (int t = 0; t <= 360 ; ++t) {
        for (radius = innerCircleRadius; radius < outerCircleRadius; ++radius){
            //        for (radius = 1; radius < src.rows; ++radius){
            X = radius * cos(float(t) * M_PI / 180.0) + palmCenter.x;
            Y = radius * sin(float(t) * M_PI / 180.0) + palmCenter.y;
            int colour = bw.at<uchar>(Point(X, Y));
            
            cout<< "AUX1: " << colour << "\n";
            //            circle(palmMask, Point(X, Y), 1, Scalar(100,0, 125));
            
            if (bw.at<uchar>(Point(X, Y)) == 255) {
                circle(palmMask, Point(X, Y), 2, Scalar(0,250, 125));
                
                //CurrentPoint is black
                int xVector[8] = {0,  0, 1, -1,  1,  1, -1, -1};
                int yVector[8] = {1, -1, 0,  0, -1, -1,  1,  1};
                bool blackFound = false;
                Point neighbour;
                for (int i = 0; i < 8; ++i){
                    neighbour = Point(X + xVector[i], Y + yVector[i]);
                    circle(palmMask, neighbour, 2, Scalar(100,0, 125));
                    
                    if (bw.at<uchar>(neighbour) == 0) {
                        blackFound = true;
                        if (neighbour.x > 0 && neighbour.x < src.rows && neighbour.y > 0 && neighbour.y < src.cols){
                            //                        filledPalmMask.at<float>(neighbour) = 255.0;
                        }
                    }
                    if (blackFound) {
                        break;
                    }
                }
                filledPalmMask.at<float>(Point(X, Y)) = 255.0;
                
                if (!blackFound) {
                    int xVector[8] = {0,  0, 1, -1,  1,  1, -1, -1};
                    int yVector[8] = {1, -1, 0,  0, -1, -1,  1,  1};
                    Point neighbour;
                    for (int i = 0; i < 8; ++i){
                        neighbour = Point(X + xVector[i], Y + yVector[i]);
                        points.push_back(neighbour);
                        //                    points[nrOfPoints] = neighbour;
                        nrOfPoints++;
                        filledPalmMask.at<float>(neighbour) = 255.0;
                    }
                }
                
            } else {
                
            }
            
        }
    }
    
    
    int levelChanges[100000];
    int i = 0;
    int arrayValue = 0;
    float lastX, lastY;
    lastX = -1;
    lastY = -1;
    bool increase = true;
    vector<Point> circle_points;
    Size axes( outerCircleRadius, outerCircleRadius);
    
    ellipse2Poly( palmCenter, axes, 0, 0, 360, 1, circle_points);
    Mat cpySrc = src.clone();
    for(int i = 0; i < circle_points.size(); i++){

        Point current_point = circle_points[i];
        circle(cpySrc, current_point, 2, Scalar(100,0, 125));

        X = current_point.x;
        Y = current_point.y;
        if (lastX != -1 && lastY != -1) {
            if (bw.at<uchar>(Point(X, Y)) == 0) {
                levelChanges[i] = 10;
            } else {
                levelChanges[i] = 1;
            }
//            if (bw.at<uchar>(Point(lastX, lastY)) != bw.at<uchar>(Point(X, Y))) {
//                increase = !increase;
//            } else {
//                if (increase) {
//                    arrayValue++;
//                } else {
//                    arrayValue--;
//                }
//
//            }
//            levelChanges[i] = arrayValue;
            i++;
        }
        lastX = X;
        lastY = Y;
        
    }
    imshow("CIRCLE ELLISE", cpySrc);
    
    //    for (int i = palmCenter.y-outerCircleRadius; i < palmCenter.y+outerCircleRadius; i++) {
    //        for (int j = palmCenter.x; (j-palmCenter.x)^2 + (i-palmCenter.y)^2 <= outerCircleRadius^2; j--) {
    //            //in the circle
    //            X = i + palmCenter.x;
    //            Y = j + palmCenter.y;
    //            if (lastX != -1 && lastY != -1) {
    //                if (bw.at<uchar>(Point(lastX, lastY)) != bw.at<uchar>(Point(X, Y))) {
    //                    if (arrayValue == 0) {
    //                        arrayValue = 1;
    //                    } else {
    //                        arrayValue = 0;
    //                    }
    //                }
    //                levelChanges[i] = arrayValue;
    //                i++;
    //            }
    //            lastX = X;
    //            lastY = Y;
    //        }
    //        lastX = -1;
    //        lastY = -1;
    //        for (int j = palmCenter.x+1; (j-palmCenter.x)*(j-palmCenter.x) + (i-palmCenter.y)*(i-palmCenter.y) <= outerCircleRadius*outerCircleRadius; j++) {
    //            //in the circle
    //            X = i;
    //            Y = j;
    //            if (lastX != -1 && lastY != -1) {
    //                if (bw.at<uchar>(Point(lastX, lastY)) != bw.at<uchar>(Point(X, Y))) {
    //                    if (arrayValue == 0) {
    //                        arrayValue = 1;
    //                    } else {
    //                        arrayValue = 0;
    //                    }
    //                }
    //                levelChanges[i] = arrayValue;
    //                i++;
    //            }
    //
    //            lastX = X;
    //            lastY = Y;
    //        }
    //
    //
    //    }
    
    displayHistogramArray(levelChanges, "Level changes");
    
    float biggestDiff = 0;
    int biggestDiffIndex = 0;
    for (int i = 0;i < points.size() - 1; ++i) {
        Point point = points[i];
        Point nextPoint = points[i + 1];
        float distance = sqrt((point.x - nextPoint.x) * (point.x - nextPoint.x) - (point.y - nextPoint.y) * (point.y - nextPoint.y));
        if (distance > biggestDiff) {
            biggestDiff = distance;
            biggestDiffIndex = i;
        }
        //        cout<< "Distance: "<< distance <<"\n";
    }
    
    circle(src, points[biggestDiffIndex], 10, CV_RGB(255,120,255));
    circle(src, points[biggestDiffIndex + 1], 10, CV_RGB(255,120,255));
    imshow("ankles ",src);
    
    imshow("filledPalmMask3 ",filledPalmMask);
    
    filledPalmMask.convertTo(filledPalmMask, CV_8U);
    filledPalmMask = 255 - filledPalmMask;
    imshow("filledPalmMask2 ",filledPalmMask);
    imshow("palmMaskCircle ",palmMask);
    
    Mat segmentedFingers;
    //    bitwise_and(src, filledPalmMask, segmentedFingers);
    src.clone().copyTo(segmentedFingers, filledPalmMask);
    imshow("segmentedFingers ",segmentedFingers);
    
    //    imshow("DIST center", dist);
    
    
    //    radius = pointPolygonTest(dist, palmCenter, false);
    //    Rect boundingRect(palmCenter.x - innerCircleRadius, palmCenter.y - innerCircleRadius, innerCircleRadius * 2 + 1, innerCircleRadius * 2 + 1);
    
    //    Circle innerCircle = Circle
    //    innercircleImage = src(boundingRect);
    //    imshow("Palm center", innercircleImage);
    
    
    //Find centroid & orientation
    
    
    
    
    
    
    // Dilate a bit the dist image
    Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
    //    dilate(dist, dist, kernel1);
    imshow("Peaks", dist);
    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    vector<vector<Point> > contours;
    findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
        drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
    // Draw the background marker
    circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);
    imshow("Markers", markers*10000);
    // Perform the watershed algorithm
    watershed(src, markers);
    Mat mark = Mat::zeros(markers.size(), CV_8UC1);
    markers.convertTo(mark, CV_8UC1);
    bitwise_not(mark, mark);
    //    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
    // image looks like at that point
    // Generate random colors
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
                dst.at<Vec3b>(i,j) = colors[index-1];
            else
                dst.at<Vec3b>(i,j) = Vec3b(0,0,0);
        }
    }
    // Visualize the final image
    imshow("Final Result", dst);
    waitKey(0);
}

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
    //    setupStartMenu();
        startCamera();
//    findHand();
    
    
    return 0;
}


