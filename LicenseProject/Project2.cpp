//
//  Project2.cpp
//  LicenseProject
//
//  Created by Vlad Bonta on 15/05/2018.
//  Copyright © 2018 Vlad Bonta. All rights reserved.
//

#include "Project2.hpp"
#include "Square.hpp"
#include "Hand.hpp"
#include "ImageRepository.hpp"

//#define COLORSPACE_TO CV_BGR2HSV
//#define COLORSPACE_ORIG CV_HSV2BGR

//#define COLORSPACE_TO CV_BGR2YUV
//#define COLORSPACE_ORIG CV_YUV2BGR

//#define COLORSPACE_TO CV_BGR2HLS
//#define COLORSPACE_ORIG CV_HLS2BGR

#define COLORSPACE_TO CV_BGR2YCrCb
#define COLORSPACE_ORIG CV_YCrCb2BGR

#define NR_OF_SAMPLES 1
using namespace cv;
using namespace std;
/*
 state
 0 - menu
 1 - start camera and place sqaures
 2 - get hand color
 3 - hand color captured, binarization of image
 4 - make contours
 5 -
 */
int state = 0;
string ROI_NAME = "Rectangle Of Interest";
string TRACKBARS_NAME = "trackbars";
string playingScreenName = "GRIP";
bool playedSoundOnce;
Size ROISize;
Size capS;
Mat roiFrame;
Mat hue;
Mat sat;
Mat coloredRoiFrame;
Mat distanceTranformImage;
Rect handDetectedRect;

Hand *rightHand;
Hand *leftHand;
vector <Vec3b> handMedianColor;

float resultTextSize = 3;
float colorEpsilon = 3.0;
float secondsLeftToDisplayHand;
int c_lower[NR_OF_SAMPLES][3];
int c_upper[NR_OF_SAMPLES][3];
void solveHandDetection(Mat img);
void findHand(Mat src);
Point handDetectedCenterPoint;

#define PI 3.14159
float findTheRadius(Mat image, bool isRightHand, vector<Point> contours);

int lastNrOfFingers;
bool answeredCorrectly = false;

Level *currentPlayingLevel;
Scalar currentLeftRectColor = Scalar(255, 0, 0);
Scalar currentRightRectColor = Scalar(255, 0, 0);

Scalar redColor = Scalar(0, 0, 255);
Scalar blueColor = Scalar(255, 0, 0);
Scalar greenColor = Scalar(0, 255, 0);
Scalar whiteColor = Scalar(255, 255, 255);
Scalar blackColor = Scalar(0, 0, 0);
Vec3b lowestColor = Vec3b(255,255,255);
Vec3b heighestColor = Vec3b(0,0,0);

Mat originalImage;


Scalar standardDeviationScalar;

int currentSample = 0;
string intToString(int number);

float distanceP2P(Point a, Point b){
    float d= sqrt(fabs( pow(a.x-b.x,2) + pow(a.y-b.y,2) )) ;
    return d;
}

void printTextOnImage(Mat src, string text){
    rectangle(src, Point(0.0, 0.0), Point(src.cols, 30.0), Scalar(255.0, 255.0, 255.0), -1);
    putText(src,text,Point(src.cols / 4.0, 20.0),1, 1.2f,Scalar(0.0, 0.0, 0.0),2);
}

void resetMedianCountFingers() {
    for (int i = 0; i < 6; ++i) {
        leftHand->handReadNumbers[i] = 0;
        rightHand->handReadNumbers[i] = 0;
    }
    
}

void placeSquares(Mat image, bool isRightHand) {
    float width = 0.0;
    float height = 0.0;
    float x = 0.0;
    float y = 0.0;
    
    if (isRightHand) {
        width = rightHand->handROI.width;
        height = rightHand->handROI.height;
        x = rightHand->handROI.x;
        y = rightHand->handROI.y;
    } else {
        width = leftHand->handROI.width;
        height = leftHand->handROI.height;
        x = leftHand->handROI.x;
        y = leftHand->handROI.y;
    }
    vector <Square> squarePoints;
    
    squarePoints.clear();
    squarePoints.push_back(Square(Point(x + width / 3, y + height / 6)));
    squarePoints.push_back(Square(Point(x + width / 3, y + height / 3)));
    squarePoints.push_back(Square(Point(x + width / 2, y + height / 3)));
    squarePoints.push_back(Square(Point(x + width / 4, y + height / 1.5)));
    squarePoints.push_back(Square(Point(x + width / 2, y + height / 1.5)));
    squarePoints.push_back(Square(Point(x + width / 5, y + height / 2.5)));
    squarePoints.push_back(Square(Point(x + width / 3, y + height / 2.0)));
    
    
    if (isRightHand) {
        rightHand->handSquarePoints = squarePoints;
    } else {
        leftHand->handSquarePoints = squarePoints;
    }
    
    string hintText = string("Acopera dreptunghurile mici cu mana si apasa tasta 'S'");
    printTextOnImage(image,hintText);
    
    for (int i = 0; i < squarePoints.size(); ++i) {
        squarePoints[i].draw_rectangle(image);
    }
    imshow(playingScreenName, image);
    moveWindow(playingScreenName, 0, 0);
}

void printScalar(Scalar scalar, String title) {
    cout<<title;
    cout <<"\n";
    cout<<scalar[0];
    cout <<" ";
    cout<<scalar[1];
    cout <<" ";
    cout<<scalar[2];
    cout <<"\n";
}

void computeMedianHandColor(Mat image, bool isRightHand) {
    //Iterate through squares to get the median color from each square
    double rColor = 0.0;
    double gColor = 0.0;
    double bColor = 0.0;
    int count = 0;
    vector <Square> squarePoints;
    if (isRightHand) {
        squarePoints = rightHand->handSquarePoints;
    } else {
        squarePoints = leftHand->handSquarePoints;
    }
    cvtColor(image, image, COLORSPACE_TO);
    
    
    standardDeviationScalar = Scalar(0.0, 0.0, 0.0);
    for (int squareIndex = 0; squareIndex < squarePoints.size(); ++squareIndex) {
        Square currentSquare = squarePoints[squareIndex];
        Point upperLeftCorner = currentSquare.upperLeftCorner;
        Point lowerLeftCorner = currentSquare.lowerLeftCorner;
        
        Rect sqaureRect = Rect(upperLeftCorner.x, upperLeftCorner.y, 18.0, 18.0);
        Mat squareImage = image(sqaureRect);
        
        Scalar mean,dev;
        meanStdDev(squareImage, mean, dev);
        
        standardDeviationScalar[0] += dev[0];
        standardDeviationScalar[1] += dev[1];
        standardDeviationScalar[2] += dev[2];
        for (int i = upperLeftCorner.x; i < lowerLeftCorner.x; ++i) {
            for (int j = upperLeftCorner.y; j < lowerLeftCorner.y; ++j) {
                Vec3b colour = image.at<Vec3b>(Point(i, j));
                if (colour[0] < lowestColor[0]) {
                    lowestColor[0] = colour[0];
                }
                if (colour[0] > heighestColor[0]) {
                    heighestColor[0] = colour[0];
                }
                
                if (colour[1] < lowestColor[1]) {
                    lowestColor[1] = colour[1];
                }
                if (colour[1] > heighestColor[1]) {
                    heighestColor[1] = colour[1];
                }
                
                if (colour[2] < lowestColor[2]) {
                    lowestColor[2] = colour[2];
                }
                if (colour[2] > heighestColor[2]) {
                    heighestColor[2] = colour[2];
                }
                
                rColor += colour[0];
                gColor += colour[1];
                bColor += colour[2];
                count++;
            }
        }
    }
    standardDeviationScalar[0] /= squarePoints.size();
    standardDeviationScalar[1] /= squarePoints.size();
    standardDeviationScalar[2] /= squarePoints.size();
    
    rColor /= count;
    gColor /= count;
    bColor /= count;
    
    Mat medianColorImage = Mat(10, 10, CV_8UC3, Scalar(rColor, gColor, bColor));
    //    cvtColor(medianColorImage, medianColorImage, COLORSPACE_TO);
    
    
    Vec3b hls = Vec3b(rColor, gColor, bColor);
    hls = medianColorImage.at<Vec3b>(0, 0);
    vector <Vec3b> medianColor;
    medianColor.push_back(hls);
    handMedianColor = medianColor;
    printScalar(Scalar(rColor, gColor, bColor), "MEDIAN COLOR ");
    
    //    imshow("Median color", medianColorImage);
    cvtColor(image, image, COLORSPACE_ORIG);
}

void createROI(Mat frame) {
    ROISize = Size(capS.width / 2.0, capS.height / 1.3);
    Rect ROI = Rect(ROISize.width, 0.0, ROISize.width, ROISize.height);
    roiFrame = frame(ROI);
    
    coloredRoiFrame = frame(ROI).clone();
}

Mat performBinarization(Mat image2, bool isRightHand) {
    Scalar lowerBoundColor = Scalar(0.0, 0.0, 0.0);
    Scalar upperBoundColor = Scalar(0.0, 0.0, 0.0);
    cvtColor(image2, image2, COLORSPACE_TO);
    //    imshow("colorspaceImage", image2);
    Mat image;
    if (isRightHand) {
        image = image2(rightHand->handROI);
        
    } else {
        image = image2(leftHand->handROI);
    }
    //        imshow("colorspaceImage2", image);
    //    resize(image, image, Size(image2.rows, image2.cols));
    
    
    //    imshow("Playing screen COLORSPACE_TO", image2);
    
    hue = Mat(image.size(), CV_8U);
    mixChannels(image, hue, {0, 0});
    
    sat = Mat(image.size(), CV_8U);
    mixChannels(image, sat, {1, 0});
    
    Mat value = Mat(image.size(), CV_8U);
    mixChannels(image, value, {2, 0});
    
    //    equalizeHist(hue, hue);
    //    equalizeHist(sat, sat);
    //    equalizeHist(value, value);
    
    vector<Mat> bwFrameList;
    
    for (int i = 0; i < NR_OF_SAMPLES;++i) {
        lowerBoundColor[0] = handMedianColor[i][0] - colorEpsilon * standardDeviationScalar[0];
        lowerBoundColor[1] = handMedianColor[i][1] - colorEpsilon * standardDeviationScalar[1];
        lowerBoundColor[2] = handMedianColor[i][2] - colorEpsilon * standardDeviationScalar[2];
        //        lowerBoundColor[1] = 77;
        //        lowerBoundColor[2] = 137;
        //        lowerBoundColor = lowestColor;
        lowerBoundColor[2] = 0.0;
        
        upperBoundColor[0] = handMedianColor[i][0] + colorEpsilon*standardDeviationScalar[0];
        upperBoundColor[1] = handMedianColor[i][1] + colorEpsilon*standardDeviationScalar[1];
        upperBoundColor[2] = handMedianColor[i][2] + colorEpsilon*standardDeviationScalar[2];
        //        upperBoundColor[1] = 120;
        //        upperBoundColor[2] = 160;
        //        upperBoundColor = heighestColor;
        upperBoundColor[2] = 255;
    }
    //cout<< "COLORS: " << lowerBoundColor[0] << " " << lowerBoundColor[1] << " " <<  upperBoundColor[0] << " " << upperBoundColor[1] << " " << "\n";
    //        cout<< "medianColor: " << medianColor[0][1] << " " << medianColor[0][2] << " " << "\n";
    //    cout<< "standardDeviationScalar: " << standardDeviationScalar[1] << " " << standardDeviationScalar[2] << " " << "\n";
    
    if(lowerBoundColor[1] < 0) {
        lowerBoundColor [1] = 0;
    }
    if(lowerBoundColor[2] < 0) {
        lowerBoundColor [2] = 0;
    }
    
    if(upperBoundColor[2] > 255) {
        upperBoundColor[2] = 255;
    }
    if(upperBoundColor[1] > 255) {
        upperBoundColor[1] = 255;
    }
    //    cout<< "COLORS: " << lowerBoundColor[1] << " " << lowerBoundColor[2] << " " <<  upperBoundColor[1] << " " << upperBoundColor[2] << " " << "\n";
    
    //    threshold(hue, hue, lowerBoundColor[0], upperBoundColor[0], CV_THRESH_BINARY);
    //    threshold(sat, sat, lowerBoundColor[1], upperBoundColor[1], CV_THRESH_BINARY);
    //    threshold(value, value, lowerBoundColor[2], upperBoundColor[2], CV_THRESH_BINARY);
    
    inRange(hue, lowerBoundColor[0], upperBoundColor[0], hue);
    inRange(sat, lowerBoundColor[1], upperBoundColor[1], sat);
    inRange(value, lowerBoundColor[2], upperBoundColor[2], value);
    
    //
    //    Mat image3 = image.clone();
    ////    cvtColor(image3, image3, COLORSPACE_ORIG);
    //imshow("imaggeeee", image3);
    //    Scalar lower = Scalar(0,0,255);
    //    Scalar upper = Scalar(360,0,255);
    //
    //    Mat mask;
    //    inRange(image3 , lowerBoundColor, upperBoundColor, mask);
    ////inRange(image3 , lower, upper, mask);
    //    imshow("maskkkk", mask);
    //    Mat res;
    //    bitwise_and(image3, image3, res, mask);
    //    imshow("andResult", res);
    
    
    
    
    //
    //
    //        imshow("hue", hue);
    //
    //        imshow("sat", sat);
    //
    //        imshow("value", value);
    
    //    //    inRange(sat, lowerBoundColor[1], upperBoundColor[1], sat);
    //    inRange(sat, lowerBoundColor[1], upperBoundColor[1], sat);
    //    inRange(value, lowerBoundColor[2], upperBoundColor[2], value);
    //    imshow("value", value);
    //    bitwise_not(value, value);
    //    imshow("notvalue", value);
    
    
    Mat hueANDSat;
    bitwise_or(sat, sat, hueANDSat);
    medianBlur(hueANDSat, image, 1);
    
    return image;
}

int findBiggestContour(vector<vector<Point> > contours) {
    int index = -1;
    float size = 0;
    for (int i = 0; i < contours.size(); ++i) {
        if (contours[i].size() > size) {
            size = contours[i].size();
            index = i;
        }
    }
    
    return index;
}

vector <Point> fingerTips;

string intToString(int number){
    stringstream ss;
    ss << number;
    string str = ss.str();
    return str;
}

int countNrOfFingers(int* histogram, String windowName, vector<Point> pointsOncircle, bool isRightHand, bool smallRadius){
    int height = 100;
    bool started = false;
    bool multipleContinuousFingersDetected = false;
    int nrOfContinuousHand = 0;
    int nrOfFingers = 0;
    float upThreshold = 28.0;
    float multipleFingersThreshold = 18;
    if (smallRadius) {
        upThreshold -= 5;
    }
    
    vector<Point> detectedFingersPositions;
    
    
    int startIndex = 0;
    int finalIndex = 0;
    int length = pointsOncircle.size();
    
    Mat histogramImage = Mat(height, length, CV_8UC3, CV_RGB(0, 0, 0));
    
    for (int i = 0; i < length; ++i){
        int limit = height / 2.0;
        if (i < length - 1) {
            if (started) {
                nrOfContinuousHand++;
            }
            if (histogram[i] != histogram[i + 1] && histogram[i + 1] != 0) {
                started = true;
                startIndex = i;
            }
            if (histogram[i] != histogram[i + 1] && histogram[i + 1] == 0) {
                started = false;
                
                finalIndex = i;
                //                cout<<"nrOfContinuousHand: "<<nrOfContinuousHand<< "\n";
                
                if (nrOfContinuousHand <= upThreshold && nrOfContinuousHand > 5.0) {
                    nrOfFingers++;
                    if (!smallRadius) {
                        int midIndex = (startIndex + finalIndex) / 2.0;
                        Point fingerPoint = pointsOncircle[midIndex];
                        detectedFingersPositions.push_back(fingerPoint);
                    }
                    
                } else if (nrOfContinuousHand >= multipleFingersThreshold && nrOfContinuousHand < multipleFingersThreshold*2 && !smallRadius) {
                    
                    int midIndex = (startIndex + finalIndex) / 3.0;
                    Point fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    midIndex = 2.0 * (startIndex + finalIndex) / 3.0;
                    fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    
                    
                    multipleContinuousFingersDetected = true;
                    nrOfFingers+=2;
                } else if (nrOfContinuousHand >= multipleFingersThreshold*2 && nrOfContinuousHand < multipleFingersThreshold*3 && !smallRadius) {
                    int midIndex = (startIndex + finalIndex) / 4.0;
                    Point fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    midIndex = 2.0 * (startIndex + finalIndex) / 4.0;
                    fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    midIndex = 3.0 * (startIndex + finalIndex) / 4.0;
                    fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    
                    multipleContinuousFingersDetected = true;
                    nrOfFingers+=3;
                } else if (nrOfContinuousHand >= multipleFingersThreshold*3 && nrOfContinuousHand < multipleFingersThreshold*4 && !smallRadius) {
                    int midIndex = (startIndex + finalIndex) / 5.0;
                    Point fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    midIndex = 2.0 * (startIndex + finalIndex) / 5.0;
                    fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    midIndex = 3.0 * (startIndex + finalIndex) / 5.0;
                    fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    midIndex = 4.0 * (startIndex + finalIndex) / 5.0;
                    fingerPoint = pointsOncircle[midIndex];
                    detectedFingersPositions.push_back(fingerPoint);
                    
                    multipleContinuousFingersDetected = true;
                    nrOfFingers+=4;
                }
                
                nrOfContinuousHand = 0;
                
            }
            
        }
        if (histogram[i] == 0) {
            limit = 1;
        }
        Point p1 = Point(i, height);
        Point p2 = Point(i, height - limit);
        
        line(histogramImage, p1, p2, CV_RGB(255, 0, 0));
    }
    //    if (!smallRadius) {
    //        imshow(windowName, histogramImage);
    //    }
    
    
    
    if (nrOfFingers > 5) {
        return -1;
    }
    if (!smallRadius) {
        if (isRightHand) {
            rightHand->detectedFingersPositions = detectedFingersPositions;
            rightHand->multipleContinuousFingersDetected = multipleContinuousFingersDetected;
        } else {
            leftHand->detectedFingersPositions = detectedFingersPositions;
            leftHand->multipleContinuousFingersDetected = multipleContinuousFingersDetected;
        }
    }
    return nrOfFingers;
}

int displayHistogramArray(int* histogram, String windowName, int length, bool isRightHand, bool smallRadius){
    int height = 100;
    Mat histogramImage = Mat(height, length, CV_8UC3, CV_RGB(0, 0, 0));
    bool started = false;
    int nrOfContinuousHand = 0;
    int nrOfFingers = 0;
    
    for (int i = 0; i < length; ++i){
        int limit = height / 2.0;
        if (i < length - 1) {
            if (started) {
                nrOfContinuousHand++;
            }
            if (histogram[i] != histogram[i + 1] && histogram[i + 1] != 0) {
                started = true;
            }
            if (histogram[i] != histogram[i + 1] && histogram[i + 1] == 0) {
                started = false;
                //                cout<<"nrOfContinuousHand: " << nrOfContinuousHand << "\n";
                if (nrOfContinuousHand < 30.0 && nrOfContinuousHand > 5.0) {
                    nrOfFingers++;
                }
                nrOfContinuousHand = 0;
            }
            
        }
        if (histogram[i] == 0) {
            limit = 1;
        }
        Point p1 = Point(i, height);
        Point p2 = Point(i, height - limit);
        
        line(histogramImage, p1, p2, CV_RGB(255, 0, 0));
    }
    if (smallRadius) {
        cout<<"Small Radius Fingers: " <<nrOfFingers << "\n\n";
    } else {
        cout<<"Radius Fingers: " <<nrOfFingers << "\n\n";
    }
    
    
    imshow(windowName, histogramImage);
    return nrOfFingers;
}

void extractImages(Mat image) {
    leftHand->handImage = image(leftHand->handROI);
    rightHand->handImage = image(rightHand->handROI);
    //    imshow("leftImage", leftImage);
    //    imshow("rightImage", rightImage);
}

Mat filterImage(Mat image) {
    Mat_<float> kernel(5,5);
    kernel <<   1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1;
    kernel <<   1, 1, 1, 1, 1,
    1, 2, 2, 2, 1,
    1, 2, 5, 2, 1,
    1, 2, 2, 2, 1,
    1, 1, 1, 1, 1;
    
    Mat_<float> kernel3x3(3,3);
    kernel3x3 <<   1, 1, 1,
    1, 1, 1,
    1, 1, 1;
    
    
    Mat filteredImage;
    //Apply 2 closes to fill holes
    morphologyEx(image, filteredImage, MORPH_CLOSE, kernel);
    morphologyEx(filteredImage, filteredImage, MORPH_CLOSE, kernel);
    morphologyEx(filteredImage, filteredImage, MORPH_CLOSE, kernel);
    
    //Apply 1 open to remove small noises
    morphologyEx(filteredImage, filteredImage, MORPH_OPEN, kernel3x3);
    
    morphologyEx(filteredImage, filteredImage, MORPH_ERODE, kernel, Point(-1, -1), 1);
    
    return filteredImage;
}

void findBiggestContourAndFillIt(Mat image, bool isRightHand) {
    Mat bluredImage = image.clone();
    
    GaussianBlur(bluredImage, bluredImage, Size(0, 0), 1);
    
    vector<vector<Point> > contours;
    findContours(bluredImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    int indexOfBiggestContour = findBiggestContour(contours);
    if (indexOfBiggestContour != -1) {
        vector<Point> biggestContour = contours[indexOfBiggestContour];
        for (int i = 0; i < contours.size(); ++i) {
            if (contours[i].size() < biggestContour.size()) {
                Rect otherRect = boundingRect(contours[i]);
                image(otherRect) = 0;
                
            }
        }
        
        handDetectedRect = boundingRect(biggestContour);
        Mat handDetectedRectImage = image.clone();
        
        rectangle(handDetectedRectImage, Point(handDetectedRect.x, handDetectedRect.y), Point(handDetectedRect.x + handDetectedRect.width, handDetectedRect.y + handDetectedRect.height), Scalar(255.0, 255.0, 0));
        
        vector<Point> hullPoints;
        vector<int> hullI;
        
        convexHull(Mat(biggestContour), hullPoints, false, true);
        convexHull(Mat(biggestContour), hullI, false, false);
        approxPolyDP(Mat(hullPoints), hullPoints, 1, true);
        if (isRightHand) {
            rightHand->handHullPoints = hullPoints;
        } else {
            leftHand->handHullPoints = hullPoints;
        }
        Mat allContoursImage = image.clone();
        
        Scalar color = Scalar( 255.0, 25.0, 10.0);
        vector<vector<Point>> vector1;
        vector1.push_back(hullPoints);
        
        vector<vector<Point>> vector2;
        vector2.push_back(biggestContour);
        
        
        Mat filledContours = allContoursImage.clone();
        
        rectangle(allContoursImage, Point(handDetectedRect.x, handDetectedRect.y), Point(handDetectedRect.x + handDetectedRect.width, handDetectedRect.y + handDetectedRect.height), Scalar(255.0, 255.0, 0));
        
        drawContours(allContoursImage, vector1, 0, color, 1);
        drawContours(allContoursImage, vector2, 0, color, 1, LINE_4);
        
        //Fill the contours to have a filled hand
        drawContours(filledContours, vector2, 0, color, -1);
        //        imshow("allcotnus", allContoursImage);
        //        imshow("filledContours", filledContours);
        if (isRightHand) {
            rightHand->handDetectedRect = handDetectedRect;
            rightHand->originalHandDetectedRect = handDetectedRect;
            rightHand->handImageFilledContours = filledContours;
            rightHand->handBiggestContour = biggestContour;
        } else {
            leftHand->handDetectedRect = handDetectedRect;
            leftHand->originalHandDetectedRect = handDetectedRect;
            leftHand->handImageFilledContours = filledContours;
            leftHand->handBiggestContour = biggestContour;
        }
        for (int i = 0; i < image.rows; ++i) {
            if (handDetectedRect.y + 10 < image.cols) {
                Point currentPoint = Point(i, handDetectedRect.y + 10);
                if (image.at<uchar>(currentPoint) != 0) {
                    if (isRightHand) {
                        rightHand->topMostHandFinger = currentPoint;
                    } else {
                        leftHand->topMostHandFinger = currentPoint;
                    }
                    break;
                }
            }
        }
    } else {
        vector<Point> biggestContour;
        biggestContour.push_back(Point(0, 0));
        if (isRightHand) {
            rightHand->handDetectedRect = handDetectedRect;
            rightHand->originalHandDetectedRect = handDetectedRect;
            rightHand->handImageFilledContours = Mat();
            rightHand->handBiggestContour = biggestContour;
        } else {
            leftHand->handDetectedRect = handDetectedRect;
            leftHand->originalHandDetectedRect = handDetectedRect;
            leftHand->handImageFilledContours = Mat();
            leftHand->handBiggestContour = biggestContour;
        }
        
    }
}

Mat computeDistanceTransform(Mat image) {
    Mat DT;
    distanceTransform(image.clone(), DT, CV_DIST_L2, 3);
    
    normalize(DT, DT, 0, 1., NORM_MINMAX);
    
    return DT;
}

void findDTCenter(Mat image, bool isRightHand) {
    handDetectedCenterPoint = Point(0, 0);
    int nrOfMaxPoints = 0;
    float maximum = -1;
    if (isRightHand) {
        handDetectedRect = rightHand->handDetectedRect;
    } else {
        handDetectedRect = leftHand->handDetectedRect;
    }
    for (int i = handDetectedRect.x; i < handDetectedRect.x + handDetectedRect.width; ++i) {
        for (int j = handDetectedRect.y; j < handDetectedRect.y + handDetectedRect.height; ++j) {
            float colour = image.at<float>(Point(i, j));
            if (colour > 0) {
                if (colour > maximum) {
                    maximum = colour;
                    nrOfMaxPoints = 1;
                    handDetectedCenterPoint = Point(i, j);
                } else if (colour == maximum) {
                    nrOfMaxPoints++;
                    handDetectedCenterPoint.x += i;
                    handDetectedCenterPoint.y += j;
                }
            }
        }
    }
    
    if (nrOfMaxPoints > 0) {
        handDetectedCenterPoint.x /= nrOfMaxPoints;
        handDetectedCenterPoint.y /= nrOfMaxPoints;
    }
    
    if (isRightHand) {
        rightHand->handDetectedCenterPoint = handDetectedCenterPoint;
    } else {
        leftHand->handDetectedCenterPoint = handDetectedCenterPoint;
    }
}

Point getMedianWristPoint(Mat image, float radius, bool isRightHand) {
    bool started = false;
    Point startingPoint = Point(0, 0);
    Point tempStartingPoint = Point(0, 0);
    Point endingPoint = Point(0, 0);
    
    int maxContinousHand = 0;
    int nrOfContinuousHand = 0;
    
    Size axes = Size(radius, radius);
    vector<Point> pointsOnCircle;
    Point centerPoint = Point(0, 0);
    if (isRightHand) {
        centerPoint = rightHand->handDetectedCenterPoint;
    } else {
        centerPoint = leftHand->handDetectedCenterPoint;
    }
    ellipse2Poly(centerPoint, axes, 0, 0, 180, 1, pointsOnCircle);
    
    Mat imageClone = image.clone();
    
    for (int i = 0; i < pointsOnCircle.size() - 1; ++i) {
        Point currentPoint = pointsOnCircle[i];
        Point nextPoint = pointsOnCircle[i + 1];
        if (i == 0) {
            circle(imageClone, currentPoint, 5, Scalar(100,255,100), 1);
        }
        if (started) {
            nrOfContinuousHand++;
        }
        if (currentPoint.x < image.cols) {
            if (!started) {
                if (currentPoint.y < image.rows && currentPoint.x < image.cols && currentPoint.x > 0 && currentPoint.y > 0) {
                    if (nextPoint.y < image.rows && nextPoint.x < image.cols && nextPoint.x > 0 && nextPoint.y > 0) {
                        if (image.at<uchar>(currentPoint) != image.at<uchar>(nextPoint) && image.at<uchar>(nextPoint) != 0) {
                            started = true;
                            tempStartingPoint = currentPoint;
                        }
                    } else {
                        started = true;
                        tempStartingPoint = currentPoint;
                    }
                } else {
                    started = true;
                    tempStartingPoint = currentPoint;
                }
            } else {
                if (currentPoint.y < image.rows && currentPoint.x < image.cols && currentPoint.x > 0 && currentPoint.y > 0) {
                    if (nextPoint.y < image.rows && nextPoint.x < image.cols && nextPoint.x > 0 && nextPoint.y > 0) {
                        if (image.at<uchar>(currentPoint) != image.at<uchar>(nextPoint) && image.at<uchar>(nextPoint) == 0) {
                            started = false;
                            maxContinousHand = nrOfContinuousHand;
                            startingPoint = tempStartingPoint;
                            endingPoint = currentPoint;
                            break;
                        }
                    }
                } else {
                    if (nextPoint.y < image.rows && nextPoint.x < image.cols && nextPoint.x > 0 && nextPoint.y > 0) {
                        started = false;
                        maxContinousHand = nrOfContinuousHand;
                        startingPoint = tempStartingPoint;
                        endingPoint = nextPoint;
                        break;
                    }
                }
            }
        }
    }
    
    if (startingPoint.x < 0) {
        startingPoint.x = 100;
    }
    if (startingPoint.y < 0) {
        startingPoint.y = 100;
    }
    if (endingPoint.x < 0) {
        endingPoint.x = 100;
    }
    if (endingPoint.y < 0) {
        endingPoint.y = 100;
    }
    
    
    if (startingPoint.x > image.rows) {
        startingPoint.x = image.rows;
    }
    if (startingPoint.y > image.cols) {
        startingPoint.y = image.cols;
    }
    
    if (endingPoint.x > image.rows) {
        endingPoint.x = image.rows;
    }
    if (endingPoint.y > image.cols) {
        endingPoint.y = image.cols;
    }
    
    Point medianPoint = Point((startingPoint.x + endingPoint.x) / 2.0, (startingPoint.y + endingPoint.y) / 2.0);
    
    circle(imageClone, startingPoint, 10, Scalar(100,255,100), 1);
    circle(imageClone, endingPoint, 20, Scalar(100,255,100), 1);
    circle(imageClone, medianPoint, 30, Scalar(100,255,100), 1);
    
    return medianPoint;
}

bool checkForZeroFingers(Mat image, bool isRightHand) {
    float hullArea = 0;
    float hullAreaWhite = 0;
    
    if (isRightHand) {
        hullArea = contourArea(rightHand->handHullPoints);
        hullAreaWhite = contourArea(rightHand->handBiggestContour);
    } else {
        if (leftHand->handHullPoints.size() == 0) {
            leftHand->handHullPoints.push_back(Point(0, 0));
        }
        if (leftHand->handBiggestContour.size() == 0) {
            leftHand->handBiggestContour.push_back(Point(0, 0));
        }
        hullArea = contourArea(leftHand->handHullPoints);
        hullAreaWhite = contourArea(leftHand->handBiggestContour);
        
    }
    
    float precentage =  100 * hullAreaWhite / hullArea;
    //    cout<<"precentage: "<<precentage<<"\n";
    if (precentage > 86.0) {
        return true;
        
    }
    return false;
}



void createPalmCircle(Mat image, bool isRightHand) {
    Point handCenter;
    float radius = 0.0;
    Rect handDetectedRect;
    if (isRightHand) {
        handCenter = rightHand->handDetectedCenterPoint;
        handCenter = rightHand->handDetectedCenterPoint;
        radius = 0.3 * (rightHand->handDetectedRect.width + rightHand->handDetectedRect.height);
        handDetectedRect = rightHand->handDetectedRect;
    } else {
        handCenter = leftHand->handDetectedCenterPoint;
        handCenter = leftHand->handDetectedCenterPoint;
        radius = 0.3 * (leftHand->handDetectedRect.width + rightHand->handDetectedRect.height);
        handDetectedRect = leftHand->handDetectedRect;
    }
    
    Point leftTopPoint = Point(handDetectedRect.x, handDetectedRect.y);
    Point leftBottomPoint = Point(handDetectedRect.x, handDetectedRect.y + handDetectedRect.height);
    Point rightTopPoint = Point(handDetectedRect.x + handDetectedRect.width, handDetectedRect.y);
    Point rightBottomPoint = Point(handDetectedRect.x + handDetectedRect.width, handDetectedRect.y + handDetectedRect.height);
    
    float dist1 = distanceP2P(handCenter, leftTopPoint);
    float dist2 = distanceP2P(handCenter, leftBottomPoint);
    float dist3 = distanceP2P(handCenter, rightTopPoint);
    float dist4 = distanceP2P(handCenter, rightBottomPoint);
    float biggestDistance = max(dist1, max(dist2, max(dist3, dist4)));
    
    
    Mat circleImage = image.clone();
    radius = biggestDistance * 0.55;
    float smallRadius = biggestDistance * 0.5;
    
    
    //    Mat im_mask= image.clone();
    //    im_mask.setTo(Scalar(0,0,0));
    //    circle(im_mask, handCenter, smallRadius, Scalar(255,255,255), -1, 8, 0);
    //    bitwise_not(im_mask, im_mask);
    //
    //    bitwise_and(image.clone(), im_mask, circleImage);
    circle(circleImage, handCenter, radius, Scalar(100,255,100), 1);
    circle(circleImage, handCenter, smallRadius, Scalar(100,255,100), 1);
    
    vector<Point> biggestContour;
    if (isRightHand) {
        biggestContour = rightHand->handBiggestContour;
    } else {
        biggestContour = leftHand->handBiggestContour;
    }
    
    
    int length = biggestContour.size();
    
    Size axes (radius,radius);
    vector<Point> pointsOnCircle;
    
    ellipse2Poly(handDetectedCenterPoint, axes, 0, 0, 360, 1, pointsOnCircle);
    
    
    int histogramArray[pointsOnCircle.size()];
    for (int i = 0; i < pointsOnCircle.size(); ++i) {
        Point currentPoint = pointsOnCircle[i];
        
        histogramArray[i] = 0.0;
        if (currentPoint.y < image.rows && currentPoint.x < image.cols && currentPoint.x > 0 && currentPoint.y > 0) {
            int colour = image.at<uchar>(currentPoint);
            histogramArray[i] = colour;
        }
    }
    
    int startIndex = -1;
    int finalIndex = -1;
    for (int i = 0; i < pointsOnCircle.size() / 2.0; ++i) {
        if ((histogramArray[i] > 0 || pointsOnCircle[i].y > image.rows) && startIndex == -1) {
            startIndex = i;
        } else if (histogramArray[i] == 0){
            if (startIndex != 0) {
                finalIndex = i;
            }
        }
    }
    int wristIndex = (startIndex + finalIndex) / 2.0;
    Point wristMidPoint = pointsOnCircle[wristIndex];
    
    if (finalIndex - startIndex > 20) {
        for (int i = startIndex; i < finalIndex; ++i) {
            histogramArray[i] = 0;
        }
        
    }
    
    
    
    
    
    int nrOfFingersBigRadius = countNrOfFingers(histogramArray, "histogramRadius", pointsOnCircle, isRightHand, false);
    
    axes = Size(smallRadius,smallRadius);
    pointsOnCircle.clear();
    
    ellipse2Poly(handDetectedCenterPoint, axes, 0, 0, 360, 1, pointsOnCircle);
    
    
    int histogramArray2[pointsOnCircle.size()];
    for (int i = 0; i < pointsOnCircle.size(); ++i) {
        Point currentPoint = pointsOnCircle[i];
        
        histogramArray2[i] = 0.0;
        if (currentPoint.y < image.rows && currentPoint.x < image.cols && currentPoint.x > 0 && currentPoint.y > 0) {
            int colour = image.at<uchar>(currentPoint);
            histogramArray2[i] = colour;
        }
    }
    
    
    startIndex = -1;
    finalIndex = -1;
    for (int i = 0; i < pointsOnCircle.size() / 2.0; ++i) {
        if ((histogramArray[i] > 0 || pointsOnCircle[i].y > image.rows) && startIndex == -1) {
            startIndex = i;
        } else if (histogramArray[i] == 0){
            if (startIndex != 0) {
                finalIndex = i;
            }
        }
    }
    wristIndex = (startIndex + finalIndex) / 2.0;
    wristMidPoint = pointsOnCircle[wristIndex];
    
    if (finalIndex - startIndex > 20) {
        for (int i = startIndex; i < finalIndex; ++i) {
            histogramArray[i] = 0;
        }
        
    }
    
    
    
    
    
    
    
    int nrOfFingersSmallRadius = countNrOfFingers(histogramArray2, "histogramRadius2", pointsOnCircle, isRightHand, true);
    
    rectangle(circleImage, Point(0.0, 0.0), Point(image.cols, 30.0), Scalar(255.0, 255.0, 255.0), -1);
    putText(circleImage,to_string(nrOfFingersSmallRadius),Point(0.0, 20.0),1, 1.2f,Scalar(0.0, 0.0, 0.0),2);
    putText(circleImage,to_string(nrOfFingersBigRadius),Point(100.0, 20.0),1, 1.2f,Scalar(150.0, 0.0, 0.0),2);
    
    bool zeroFinger = checkForZeroFingers(image.clone(), isRightHand);
    
    int nrOfFingers = 0;
    Point centerPoint = Point(0, 0);
    Point topMostPoint = Point(0, 0);
    Rect rect = Rect();
    bool multipleContinuousFingersDetected = false;
    
    if (isRightHand) {
        centerPoint = rightHand->handDetectedCenterPoint;
        topMostPoint = rightHand->topMostHandFinger;
        rect = rightHand->handDetectedRect;
        multipleContinuousFingersDetected = rightHand->multipleContinuousFingersDetected;
        
    } else {
        centerPoint = leftHand->handDetectedCenterPoint;
        topMostPoint = leftHand->topMostHandFinger;
        rect = leftHand->handDetectedRect;
        multipleContinuousFingersDetected = leftHand->multipleContinuousFingersDetected;
    }
    
    if (zeroFinger && !multipleContinuousFingersDetected) {
        nrOfFingers = 0;
    } else {
        nrOfFingers = nrOfFingersBigRadius;
        if (nrOfFingersSmallRadius == 5 && nrOfFingersBigRadius == 4) {
            nrOfFingers = 5;
        }
    }
    putText(circleImage,to_string(nrOfFingers),Point(200.0, 20.0),1, 1.2f,Scalar(150.0, 100.0, 230.0),2);
    
    if (isRightHand) {
        rightHand->nrOfFingers = nrOfFingers;
//        imshow("Right Hand", circleImage);
    } else {
        leftHand->nrOfFingers = nrOfFingers;
//        imshow("Left Hand", circleImage);
    }
}

void removeWrist(Mat image, bool isRightHand) {
    float smallestDistance = 0;
    Point minDistancePoint = Point(0,0);
    vector<Point> biggestContour;
    Point center = Point(0, 0);
    if (isRightHand) {
        biggestContour = rightHand->handBiggestContour;
        center = rightHand->handDetectedCenterPoint;
    } else {
        biggestContour = leftHand->handBiggestContour;
        center = leftHand->handDetectedCenterPoint;
    }
    
    
    for (int i = 0; i < biggestContour.size(); ++i) {
        Point currentPoint = biggestContour[i];
        
        if (currentPoint.y < image.rows && currentPoint.x < image.cols && currentPoint.x > 0 && currentPoint.y > 0) {
            if (currentPoint.y > center.y) {
                float distance = distanceP2P(currentPoint, center);
                if (distance < smallestDistance || smallestDistance == 0) {
                    minDistancePoint = currentPoint;
                    smallestDistance = distance;
                }
            }
        }
    }
    
    Size axes = Size(smallestDistance, smallestDistance);
    vector<Point> pointsOnCircle;
    
    ellipse2Poly(center, axes, 0, 0, 360, 1, pointsOnCircle);
    Point smallestPoint = Point(0, 0);
    for (int i = 0; i < pointsOnCircle.size() - 1; ++i) {
        Point currentPoint = pointsOnCircle[i];
        if (currentPoint.y < image.rows && currentPoint.x < image.cols && currentPoint.x > 0 && currentPoint.y > 0) {
            if (smallestPoint.y < currentPoint.y) {
                smallestPoint = currentPoint;
            }
        }
    }
    
    
    Point medianPoint = getMedianWristPoint(image.clone(), smallestDistance, isRightHand);
    medianPoint = smallestPoint;
    
    Rect bottomWristRect = cvRect(0, medianPoint.y, image.cols, image.rows - medianPoint.y);
    if (isRightHand) {
        rightHand->handDetectedRect.height -= bottomWristRect.height;
    } else {
        leftHand->handDetectedRect.height -= bottomWristRect.height;
    }
    Mat wholeImage = Mat(image.rows, image.cols, CV_8UC1, Scalar(255,255,255));
    wholeImage(bottomWristRect) = 0.0;
    
    bitwise_and(wholeImage, image, image);
}

void drawROIS(Mat image) {
    ROISize = Size(capS.width, capS.height);
    
    float xDistance = 10.0;
    float yDivisor = 5.0;
    float widthDivisor = 2.0;
    float heightDivisor = 3.0;
    
    leftHand->handROI = Rect(xDistance, ROISize.height / yDivisor, ROISize.height / widthDivisor, ROISize.width / heightDivisor);
    rightHand->handROI = Rect((ROISize.width - xDistance) - ROISize.height / widthDivisor, ROISize.height / yDivisor, ROISize.height / widthDivisor, ROISize.width / heightDivisor);
    
    
    
    rectangle(image, leftHand->handROI, currentLeftRectColor);
    rectangle(image, rightHand->handROI, currentRightRectColor);
}

void test(Mat image) {
    
    cvtColor(image, image, COLORSPACE_TO);
    //    //    cvtColor(image3, image3, COLORSPACE_ORIG);
    //    imshow("imaggeeee", image);
    //    Scalar lower = Scalar(0,0,240);
    //    Scalar upper = Scalar(180,255,255);
    //
    //    Mat mask;
    //    inRange(image , lower, upper, mask);
    //
    //    imshow("maskkkk", mask);
    //    Mat res;
    //    bitwise_and(image, image, res, mask);
    //    imshow("andResult", res);
    //    Mat lower_red_hue_range;
    //    inRange(image, Scalar(0, 0, 0), Scalar(180, 255, 10), lower_red_hue_range);
    //    imshow("ACI", lower_red_hue_range);
    //    inRange(image, Scalar(160, 100, 100), ScalarScalar(179, 255, 255), upper_red_hue_range);
    
    
    
    
    imshow("ycrcb", image);
    
    Mat hue = Mat(image.size(), CV_8U);
    mixChannels(image, hue, {0, 0});
    //    equalizeHist(hue, hue);
    Mat sat = Mat(image.size(), CV_8U);
    mixChannels(image, sat, {1, 0});
    //    equalizeHist(sat, sat);
    Mat value = Mat(image.size(), CV_8U);
    mixChannels(image, value, {2, 0});
    //    equalizeHist(value, value);
    //    imshow("hue1", hue);
    //
    //    imshow("sat1", sat);
    //
    //    imshow("value1", value);
    
    threshold(hue, hue, 0, 0, CV_THRESH_OTSU);
    threshold(sat, sat, 77, 120, CV_THRESH_OTSU);
    threshold(value, value, 137, 163, CV_THRESH_OTSU);
    
    equalizeHist(hue, hue);
    equalizeHist(sat, sat);
    equalizeHist(value, value);
    
    imshow("hue", hue);
    
    imshow("sat", sat);
    
    imshow("value", value);
    
    Mat result;
    vector<Mat> channels;
    channels.push_back(hue);
    channels.push_back(sat);
    channels.push_back(value);
    
    merge(channels, result);
    imshow("RESULT", result);
}

Mat addImagesToExercise(Rect exercisesRect, float nrOfElements, Mat image) {
    int containerImageWidth = exercisesRect.width;
    int containerImageHeight = exercisesRect.height;
    if (nrOfElements > 0) {
        
        int imageWidth = containerImageWidth / ((nrOfElements + 1) / 2.0);
        if (nrOfElements == 1) {
            imageWidth = containerImageWidth / 2.0;
        }
        int imageHeight = containerImageHeight / 2;
        Mat containerImage = Mat(containerImageHeight, containerImageWidth, CV_8UC3, Scalar(255,255,255));
        resize(image, image, Size(imageWidth, imageHeight));
        
        Rect imageRect = Rect(0, 0, imageWidth, imageHeight);
        for (int i = 0; i < nrOfElements; ++i) {
            image.copyTo(containerImage(imageRect));
            imageRect.x += imageWidth;
            if (imageRect.x + imageRect.width > exercisesRect.width) {
                imageRect.x = 0.0;
                imageRect.y += imageHeight;
            }
            if (imageRect.y + imageRect.height> containerImage.rows) {
                break;
            }
            //            if (imageRect.x  + imageRect.width > exercisesRect.width || imageRect.y  + imageRect.height> exercisesRect.height) {
            //                cout<<"No more space available\n";
            //                break;
            //            }
        }
        return containerImage;
    } else {
        
        Mat containerImage = Mat(1, 1, CV_8UC3, Scalar(255,255,255));
        return containerImage;
    }
}

void drawOnScene(Mat image, int totalNumberOfFingers) {
    int textRectHeight = 50;
    int textYCenter = 2.0 * textRectHeight / 3.0;
    
    
    rectangle(image, Point(0.0, 0.0), Point(image.cols, 10 + textRectHeight), whiteColor, -1);
    string result = "Raspuns: " + to_string(totalNumberOfFingers);
    putText(image,result,Point(image.cols / 2.0 - 100.0, textYCenter),1,resultTextSize, blackColor,1.5);
    
    double alpha = 0.3;
    Mat overlay;
    image.copyTo(overlay);
    
    rectangle(image, Point(rightHand->handROI.x, rightHand->handROI.y - textRectHeight), Point(rightHand->handROI.x + rightHand->handROI.width, rightHand->handROI.y), whiteColor, -1);
    putText(image,to_string(rightHand->nrOfFingers),Point(rightHand->handROI.x + rightHand->handROI.width / 2.0, rightHand->handROI.y - textRectHeight + textYCenter),1, 2.0f,blackColor,2);
    //    addWeighted(overlay, alpha, image, 1 - alpha, 0, image);
    
    rectangle(image, Point(leftHand->handROI.x, leftHand->handROI.y - textRectHeight), Point(leftHand->handROI.x + leftHand->handROI.width, leftHand->handROI.y), whiteColor, -1);
    putText(image,to_string(leftHand->nrOfFingers),Point(leftHand->handROI.x + leftHand->handROI.width / 2.0, leftHand->handROI.y - textRectHeight + textYCenter),1, 2.0f,blackColor,2);
    
    
    result = to_string((int)secondsLeftToDisplayHand);
    string remainingSecondsString = " secunde ramase";
    
    rectangle(image, Point(0.0, image.rows - 2 * textRectHeight), Point(image.cols, image.rows), whiteColor, -1);
    putText(image,result,Point(150, image.rows - textRectHeight),1,3.0f, blackColor,3);
    putText(image,remainingSecondsString,Point(0.0, image.rows + textYCenter - textRectHeight),1,2.0f, blackColor,2);
    addWeighted(overlay, alpha, image, 1 - alpha, 0, image);
    
    Sublevel *currentSublevel = currentPlayingLevel->sublevels[currentPlayingLevel->currentSublevelIndex];

    if (currentPlayingLevel->levelImage.data != NULL) {
        float wholeExerciseWidth = image.cols / 2.0;
        float wholeExerciseHeight = 100.0;
        float operandWidth = wholeExerciseWidth / (currentSublevel->operands.size() + currentSublevel->operators.size()) ;
        Rect wholeExercisesRect = Rect(image.cols / 4.0, image.rows - wholeExerciseHeight, wholeExerciseWidth, wholeExerciseHeight);
        
        Rect operandRect = Rect(0, 0.0, operandWidth, wholeExerciseHeight);
        Rect operatorRect = Rect(0, 0, operandWidth, wholeExerciseHeight);
        float currentX = 0;
        
        Mat wholeExerciseBackgroundImage = loadMenuBackground();
        resize(wholeExerciseBackgroundImage, wholeExerciseBackgroundImage, wholeExercisesRect.size());
        
        Rect exercisesRect = Rect(0, image.rows - textRectHeight + textYCenter, image.cols, 50);
        
        Mat appleImage = currentPlayingLevel->levelImage;
        int nrOfOperands = currentSublevel->operands.size();

        for (int i = 0;i < nrOfOperands; ++i) {
            int operand = currentSublevel->operands[i];

            Mat operandImage = addImagesToExercise(operandRect, operand, appleImage);
            
            resize(operandImage, operandImage, operandRect.size());
            operandImage.copyTo(wholeExerciseBackgroundImage(operandRect));
            
            currentX += operandRect.width;
            operatorRect.x = currentX;
            if (i != nrOfOperands - 1){
                string operatorString = currentSublevel->operators[i];
                putText(wholeExerciseBackgroundImage(operatorRect), operatorString, Point(0.0, 100.0), FONT_HERSHEY_PLAIN, 10, Scalar(0,0,0), 6);
            }
            
            currentX += operatorRect.width;
            operandRect.x = currentX;
        }
       
        wholeExerciseBackgroundImage.copyTo(image(wholeExercisesRect));
        
    } else {
        
        putText(image, currentPlayingLevel->sublevels[currentPlayingLevel->currentSublevelIndex]->sublevelExercise,Point(image.cols / 3.0, image.rows - textRectHeight + textYCenter),1,5.0f, blackColor,5);
    }
    if (!playedSoundOnce) {
        playExercise(currentPlayingLevel->sublevels[currentPlayingLevel->currentSublevelIndex]->sublevelExercise);
        playedSoundOnce = true;
    }
    
    
    if (answeredCorrectly) {
        putText(image,"BRAVOOO",Point(0.0, 25.0),1,2.0, blackColor,1.5);
    } else {
        putText(image,"UPS",Point(0.0, 25.0),1,2.0, blackColor,1.5);
    }
    
    
    //Display found fingers text
    
    vector<Point> fingersPoints = rightHand->detectedFingersPositions;
    for (int i = 0; i < fingersPoints.size(); ++i) {
        Point currentFingerPoint = fingersPoints[i];
        currentFingerPoint.x += rightHand->handROI.x;
        currentFingerPoint.y += rightHand->handROI.y;
        circle(image, currentFingerPoint, 5, redColor, -1);
        //        putText(image,intToString(i),currentFingerPoint,1,2.0f, blackColor,2);
    }
    fingersPoints = leftHand->detectedFingersPositions;
    for (int i = 0; i < fingersPoints.size(); ++i) {
        Point currentFingerPoint = fingersPoints[i];
        currentFingerPoint.x += leftHand->handROI.x;
        currentFingerPoint.y += leftHand->handROI.y;
        circle(image, currentFingerPoint, 5, redColor, -1);
        
        //        putText(image,intToString(i),currentFingerPoint,1,2.0f, blackColor,2);
    }
    int size = rightHand->handBiggestContour.size();
    for (int i = 0; i < size; ++i) {
        Point newPoint = rightHand->handBiggestContour[i];
        newPoint.x += rightHand->handROI.x;
        newPoint.y += rightHand->handROI.y;
        rightHand->handBiggestContour[i] = newPoint;
    }
    
    size = leftHand->handBiggestContour.size();
    for (int i = 0; i < size; ++i) {
        Point newPoint = leftHand->handBiggestContour[i];
        newPoint.x += leftHand->handROI.x;
        newPoint.y += leftHand->handROI.y;
        leftHand->handBiggestContour[i] = newPoint;
    }
    
    vector<vector<Point>> vector;
    vector.push_back(rightHand->handBiggestContour);
    drawContours(image, vector, 0, blueColor, 1);
    
    vector.clear();
    vector.push_back(leftHand->handBiggestContour);
    drawContours(image, vector, 0, blueColor, 1);
    
    
    imshow(playingScreenName, image);
    moveWindow(playingScreenName, 0, 0);
}

void manageResponse(Mat image, int totalNumberOfFingers) {
    int sublevelCheckIndex = currentPlayingLevel->currentSublevelIndex - 1;
    if (totalNumberOfFingers == currentPlayingLevel->sublevels[sublevelCheckIndex]->sublevelResponse) {
        cout<<"ANSERWEWSDSADAS" << "\n\n\n";
        answeredCorrectly = true;
        currentPlayingLevel->score++;
    } else {
        answeredCorrectly = false;
    }
    drawOnScene(image, totalNumberOfFingers);
}

void validateResponse () {
    
}

int newFrame(VideoCapture cap, Mat frame, int state, bool updateResult, int secondsLeft) {
    secondsLeftToDisplayHand = secondsLeft;
    capS = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
                (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    flip(frame, frame, 1);
    rightHand->detectedFingersPositions.clear();
    leftHand->detectedFingersPositions.clear();
    //        test(frame.clone());
    
    blur(frame, frame, Size(2, 2));
    Mat flippedFrame = frame.clone();
    originalImage = frame.clone();
    drawROIS(frame);
    extractImages(originalImage);
    
    if (state == 1) {
        Mat rectanglesImage = frame.clone();
        placeSquares(rectanglesImage, true);
        //        placeSquares(rectanglesImage, false);
    }
    
    if (state == 2) {
        computeMedianHandColor(flippedFrame, true);
        //        computeMedianHandColor(flippedFrame, false);
        currentSample++;
        if (currentSample == NR_OF_SAMPLES) {
            state = 3;
        }
        
    }
    
    if (state >= 3) {
        //            destroyWindow("Rectangle Image");
        
        roiFrame = rightHand->handImage;
        rightHand->handImageBinarized = performBinarization(flippedFrame.clone(), true);
        rightHand->handImageBinarized = filterImage(rightHand->handImageBinarized);
        if (rightHand->handImageBinarized.data) {
            findBiggestContourAndFillIt(rightHand->handImageBinarized, true);
            rightHand->handImageDT = computeDistanceTransform(rightHand->handImageFilledContours);
            if (rightHand->handImageDT.data) {
                findDTCenter(rightHand->handImageDT, true);
                if (rightHand->handImageFilledContours.data) {
                    createPalmCircle(rightHand->handImageFilledContours, true);
                } else {
                    rightHand->nrOfFingers = 0;
                    rightHand->detectedFingersPositions.clear();
                }
            } else {
                rightHand->nrOfFingers = 0;
                rightHand->detectedFingersPositions.clear();
            }
        }
        
        roiFrame = leftHand->handImage;
        leftHand->handImageBinarized = performBinarization(flippedFrame.clone(), false);
        leftHand->handImageBinarized = filterImage(leftHand->handImageBinarized);
        if (leftHand->handImageBinarized.data) {
            findBiggestContourAndFillIt(leftHand->handImageBinarized, false);
            leftHand->handImageDT = computeDistanceTransform(leftHand->handImageBinarized);
            findDTCenter(leftHand->handImageDT, false);
            if (leftHand->handImageFilledContours.data) {
                createPalmCircle(leftHand->handImageFilledContours, false);
            } else {
                leftHand->nrOfFingers = 0;
                leftHand->detectedFingersPositions.clear();
            }
        }
        int totalNrOfFingers = 0;
        if (rightHand->nrOfFingers >= 0) {
            //            if (rightHand->originalHandDetectedRect.width > 150 && rightHand->originalHandDetectedRect.height > 150) {
            currentRightRectColor = greenColor;
            totalNrOfFingers += rightHand->nrOfFingers;
            if (secondsLeft < currentPlayingLevel->seconds / 2.0) {
                rightHand->handReadNumbers[rightHand->nrOfFingers]++;
            }
            //            } else {
            //                if (rightHand->originalHandDetectedRect.width < 80 || rightHand->originalHandDetectedRect.height < 70) {
            //                    currentRightRectColor = redColor;
            //                } else {
            //                    currentRightRectColor = greenColor;
            //                }
            //                rightHand->nrOfFingers = 0;
            //            }
        } else {
            currentRightRectColor = redColor;
            rightHand->nrOfFingers = 0;
        }
        
        if (leftHand->nrOfFingers >= 0) {
            if (leftHand->originalHandDetectedRect.width > 150 && leftHand->originalHandDetectedRect.height > 150) {
                currentLeftRectColor = greenColor;
                totalNrOfFingers += leftHand->nrOfFingers;
                if (secondsLeft < currentPlayingLevel->seconds / 2.0) {
                    leftHand->handReadNumbers[leftHand->nrOfFingers]++;
                }
                
            } else {
                if (leftHand->originalHandDetectedRect.width < 80 || leftHand->originalHandDetectedRect.height < 70) {
                    currentLeftRectColor = redColor;
                } else {
                    currentLeftRectColor = greenColor;
                }
                leftHand->nrOfFingers = 0;
            }
        } else {
            currentLeftRectColor = redColor;
            leftHand->nrOfFingers = 0;
        }
        
        if (updateResult) {
            resultTextSize = 3;
            int rightFingerMaxIndex = 0;
            for (int i = 0; i < 6; ++i) {
                if (rightHand->handReadNumbers[i] >rightHand->handReadNumbers[rightFingerMaxIndex] && rightHand->handReadNumbers[i] > 10) {
                    rightFingerMaxIndex = i;
                }
//                cout << "PROBRight: "<< i << ":  " << rightHand->handReadNumbers[i] <<"\n";
            }
            
            cout << "\n";
            
            int leftFingerMaxIndex = 0;
            
            for (int i = 0; i < 6; ++i) {
                if (leftHand->handReadNumbers[i] > leftHand->handReadNumbers[leftFingerMaxIndex] && leftHand->handReadNumbers[i] > 10) {
                    leftFingerMaxIndex = i;
                }
//                cout << "PROBLeft: "<< i << ":  " << leftHand->handReadNumbers[i] <<"\n";
            }
            resetMedianCountFingers();

            totalNrOfFingers = rightFingerMaxIndex + leftFingerMaxIndex;
            
            manageResponse(frame, totalNrOfFingers);
            validateResponse();
            
            lastNrOfFingers = totalNrOfFingers;
            playedSoundOnce = false;
        } else {
            if (resultTextSize > 2) {
                resultTextSize-=0.1;
            }
            drawOnScene(frame, lastNrOfFingers);
        }
        
        
        state = 4;
    }
    
    return state;
}

void updatePlayingLevel(Level *currentLevel) {
    currentPlayingLevel = currentLevel;
}

void setupLevel(Level *level) {
    playedSoundOnce = false;

    rightHand = new Hand;
    leftHand = new Hand;
    
    currentSample = 0;
    currentRightRectColor = blueColor;
    currentLeftRectColor = blueColor;
    currentPlayingLevel = level;
    resetMedianCountFingers();
    answeredCorrectly = false;
    currentPlayingLevel->score = 0;
}
