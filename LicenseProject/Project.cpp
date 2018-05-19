//
//  Project.cpp
//  IP2017
//
//  Created by Vlad Bonta on 10/04/2017.
//  Copyright © 2017 Vlad Bonta. All rights reserved.
//

#include "Project.hpp"
#include "Square.hpp"

#define COLORSPACE_TO CV_BGR2HSV
#define COLORSPACE_ORIG CV_HSV2BGR

//#define COLORSPACE_TO CV_BGR2HLS
//#define COLORSPACE_ORIG CV_HLS2BGR

//#define COLORSPACE_TO CV_BGR2YCrCb
//#define COLORSPACE_ORIG CV_YCrCb2BGR
#define NR_OF_SAMPLES 5
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
Size ROISize;
Size capS;
Mat roiFrame;
Mat hue;
Mat sat;
Mat coloredRoiFrame;
Mat distanceTranformImage;
Rect handDetectedRect;
void solveHandDetection(Mat img);
void findHand(Mat src);
Point handDetectedCenterPoint;
#define PI 3.14159

vector <Square> squarePoints;
vector <Vec3b> medianColor;
Scalar standardDeviationScalar;

int currentSample = 0;
Mat finalBWFrame;
int binarizationOffsetValue = 25.0;
Mat workingHandImage;
string intToString(int number);

void printTextOnImage(Mat src, string text){
    rectangle(src, Point(0.0, 0.0), Point(capS.width / 2.0, 30.0), Scalar(255.0, 255.0, 255.0), -1);
    putText(src,text,Point(0.0, 20.0),1, 1.2f,Scalar(0.0, 0.0, 0.0),2);
    
}

void placeSquares() {
    float width = ROISize.width;
    float height = ROISize.height;
    
    squarePoints.clear();
    squarePoints.push_back(Square(Point(width / 3, height / 4)));
    squarePoints.push_back(Square(Point(width / 4, height / 2)));
    squarePoints.push_back(Square(Point(width / 3, height / 1.5)));
    squarePoints.push_back(Square(Point(width / 2, height / 2)));
    squarePoints.push_back(Square(Point(width / 2.5, height / 2.5)));
    squarePoints.push_back(Square(Point(width / 2, height / 1.5)));
    squarePoints.push_back(Square(Point(width / 2.5, height / 1.8)));
    
    string hintText = string("Cover rectangles with palm and press 'S' to save hand color");
    printTextOnImage(roiFrame,hintText);
    
    for (int i = 0; i < squarePoints.size(); ++i) {
        squarePoints[i].draw_rectangle(roiFrame);
    }
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

void computeMedianHandColor() {
    //Iterate through squares to get the median color from each square
    double rColor = 0.0;
    double gColor = 0.0;
    double bColor = 0.0;
    int count = 0;
    cvtColor(roiFrame, roiFrame, COLORSPACE_TO);
    
    standardDeviationScalar = Scalar(0.0, 0.0, 0.0);
    for (int squareIndex = 0; squareIndex < squarePoints.size(); ++squareIndex) {
        Square currentSquare = squarePoints[squareIndex];
        Point upperLeftCorner = currentSquare.upperLeftCorner;
        Point lowerLeftCorner = currentSquare.lowerLeftCorner;
        
        Rect sqaureRect = Rect(upperLeftCorner.x, upperLeftCorner.y, 18.0, 18.0);
        Mat squareImage = roiFrame(sqaureRect);
        
        Scalar mean,dev;
        
        meanStdDev(squareImage, mean, dev);
//        standardDeviationScalar[0] += mean[0];
//        standardDeviationScalar[1] += mean[1];
//        standardDeviationScalar[2] += mean[2];
        
                standardDeviationScalar[0] += dev[0];
                standardDeviationScalar[1] += dev[1];
                standardDeviationScalar[2] += dev[2];
        for (int i = upperLeftCorner.x; i < lowerLeftCorner.x; ++i) {
            for (int j = upperLeftCorner.y; j < lowerLeftCorner.y; ++j) {
                Vec3b colour = roiFrame.at<Vec3b>(Point(i, j));
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
    
    Vec3b hls = Vec3b(rColor, gColor, bColor);
    medianColor.push_back(hls);
    
    cvtColor(roiFrame, roiFrame, COLORSPACE_ORIG);
}

void createROI(Mat frame) {
    ROISize = Size(capS.width / 2.0, capS.height / 1.3);
    Rect ROI = Rect(ROISize.width, 0.0, ROISize.width, ROISize.height);
    roiFrame = frame(ROI);
    
    coloredRoiFrame = frame(ROI).clone();
    imshow("coloredRoiFrame", coloredRoiFrame);
}


void initTrackbars() {
    //    namedWindow(TRACKBARS_NAME, CV_WINDOW_FREERATIO);
    //    createTrackbar("BinaryOffsetValue", TRACKBARS_NAME, &binarizationOffsetValue, 100);
    
}

void performBinarization() {
    Scalar lowerBoundColor = Scalar(0.0, 0.0, 0.0);
    Scalar upperBoundColor = Scalar(0.0, 0.0, 0.0);
    
    cvtColor(roiFrame, roiFrame, COLORSPACE_TO);
    imshow("HSV", roiFrame);
    
    
    
    
    hue = Mat(roiFrame.size(), CV_8U);
    
    mixChannels(roiFrame, hue, {0, 0});
    imshow("hue", hue);
    
    sat = Mat(roiFrame.size(), CV_8U);
    
    mixChannels(roiFrame, sat, {1, 0});
    imshow("sat", sat);
    
    
    Mat value = Mat(roiFrame.size(), CV_8U);
    
    mixChannels(roiFrame, value, {2, 0});

//    imshow("value", value);

//    Mat otsuMat;
//    threshold(hue, otsuMat, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
//
//    imshow("otsuMat", otsuMat);
    
    
//    Mat hueSat = Mat(roiFrame.size(), CV_8UC3);
//
//    mixChannels(roiFrame, hueSat, {0, 0});
//    mixChannels(roiFrame, hueSat, {1, 1});
//
//    mixChannels(roiFrame, Mat(hueSat.rows, hueSat.cols, CV_8U, Scalar(0,0,0)), {2, 2});
//    imshow("hueSat", hueSat);
    
    
    vector<Mat> bwFrameList;
    
    float k = 2.2;
    for (int i = 0; i < NR_OF_SAMPLES;++i) {
        lowerBoundColor[0] = medianColor[i][0] - k*standardDeviationScalar[0];
        lowerBoundColor[1] = medianColor[i][1] - k*standardDeviationScalar[1];
        lowerBoundColor[2] = medianColor[i][2] - k*standardDeviationScalar[2];
//        lowerBoundColor[2] = medianColor[i][2] - 50.0;
//        lowerBoundColor[2] = 80.0;
        lowerBoundColor[2] = 0.0;
        
        upperBoundColor[0] = medianColor[i][0] + k*standardDeviationScalar[0];
        upperBoundColor[1] = medianColor[i][1] + k*standardDeviationScalar[1];
        upperBoundColor[2] = medianColor[i][2] + k*standardDeviationScalar[2];
        upperBoundColor[2] = 255.0;
//        upperBoundColor[2] = medianColor[i][2] + 50.0;
        Mat bwFrame;
        inRange(roiFrame.clone(), lowerBoundColor, upperBoundColor, bwFrame);
        bwFrameList.push_back(bwFrame);
    }
    Mat hueSegmented;
    inRange(hue.clone(), lowerBoundColor[0], upperBoundColor[0], hueSegmented);
//        threshold(hue.clone(), hueSegmented, lowerBoundColor[0], upperBoundColor[0], CV_THRESH_OTSU + CV_THRESH_BINARY);

    Mat satSegmented;
    inRange(sat.clone(), lowerBoundColor[1], upperBoundColor[1], satSegmented);
//    threshold(sat.clone(), satSegmented, lowerBoundColor[1], upperBoundColor[1], CV_THRESH_OTSU + CV_THRESH_BINARY);

    
    Mat valueSegmented;
    inRange(value.clone(), lowerBoundColor[2], upperBoundColor[2], valueSegmented);
//    threshold(value.clone(), ssatSegmented, lowerBoundColor[2], upperBoundColor[2], CV_THRESH_OTSU + CV_THRESH_BINARY);
//
//    imshow("huesegmented", hueSegmented);
//    imshow("satsegmented", satSegmented);
//    imshow("valuesegmented", valueSegmented);
    Mat_<float> kernel(5,5);
    kernel <<   1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1;
//    kernel <<   0, 0, 1, 0, 0,
//    1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1,
//    0, 0, 1, 0, 0;
    
//    erode(satSegmented, satSegmented, kernel);
//    dilate(satSegmented, satSegmented, kernel);
//    satSegmented = hueSegmented;
//    morphologyEx(satSegmented, satSegmented, MORPH_OPEN, kernel);
//
//    morphologyEx(satSegmented, satSegmented, MORPH_CLOSE, kernel);
//        dilate(satSegmented, satSegmented, kernel);
//        dilate(satSegmented, satSegmented, kernel);
//    morphologyEx(satSegmented, satSegmented, MORPH_OPEN, kernel);
//
//    morphologyEx(satSegmented, satSegmented, MORPH_CLOSE, kernel);
//
//    morphologyEx(satSegmented, satSegmented, MORPH_OPEN, kernel);
//
//    morphologyEx(satSegmented, satSegmented, MORPH_CLOSE, kernel);

//    imshow("satsegmentedAfter morphEx", satSegmented);

    

//    threshold(hue.clone(), hueSegmented, lowerBoundColor[0], upperBoundColor[0], CV_THRESH_OTSU + CV_THRESH_BINARY);
//    threshold(sat.clone(), satSegmented, lowerBoundColor[1], upperBoundColor[1], CV_THRESH_OTSU + CV_THRESH_BINARY);
//    threshold(ssat.clone(), ssatSegmented, lowerBoundColor[1], upperBoundColor[1], CV_THRESH_OTSU + CV_THRESH_BINARY);
//    imshow("huesegmented2", hueSegmented);
//    imshow("satsegmented2", satSegmented);
//    imshow("ssatsegmented2", ssatSegmented);

    Mat hueANDSat;
    bitwise_and(satSegmented, hueSegmented, hueANDSat);
    dilate(hueANDSat, hueANDSat, kernel);

    morphologyEx(hueANDSat, hueANDSat, MORPH_OPEN, kernel);
//
//    morphologyEx(hueANDSat, hueANDSat, MORPH_CLOSE, kernel);

//    imshow("hueANDSat", hueANDSat);

//    bitwise_and(hueANDSat, satSegmented, hueANDSat);
    imshow("ALLLTHREE", hueANDSat);

    
    Mat empty_image = Mat(hue.rows, hue.cols, CV_8U, Scalar(0,0,0));


    Mat hueAndSat;
    vector<Mat> channels;
    channels.push_back(hue);
    channels.push_back(valueSegmented);
    channels.push_back(empty_image);

    merge(channels, hueAndSat);
    
    upperBoundColor[2] = 0.0;
    lowerBoundColor[2] = 0.0;
    inRange(hueAndSat.clone(), lowerBoundColor, upperBoundColor, hueAndSat);


//    imshow("hueSatSegmented1", hueAndSat);

//    Mat hueSatSegmented;
//
//    inRange(hueAndSat.clone(), lowerBoundColor, upperBoundColor, hueSatSegmented);
//    imshow("hueSatSegmented2", hueSatSegmented);
    
    
    finalBWFrame = bwFrameList[0];
    
    for (int i = 1; i < NR_OF_SAMPLES;++i) {
        finalBWFrame += bwFrameList[i];
    }
    
    medianBlur(finalBWFrame, roiFrame, 7);
    
//    medianBlur(satSegmented, roiFrame, 1);
    medianBlur(hueANDSat, roiFrame, 1);

    imshow("finalBWFrame", roiFrame);
    
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

Mat removeNoiseOutside(Mat workingImage, Rect rect) {
    //Create a deep copy of the image
    cv::Mat fill(workingImage.clone());
    //Specify the ROI
    cv::Mat fillROI = fill(rect);
    //Fill the ROI with black
    fillROI = cv::Scalar(0);
    
    //create a deep copy of the image
    cv::Mat inverseFill(workingImage.clone());
    //create a single-channel mask the same size as the image filled with 1
    cv::Mat inverseMask(inverseFill.size(), CV_8UC1, cv::Scalar(1));
    //Specify the ROI in the mask
    cv::Mat inverseMaskROI = inverseMask(rect);
    //Fill the mask's ROI with 0
    inverseMaskROI = cv::Scalar(0);
    //Set the image to 0 in places where the mask is 1
    inverseFill.setTo(cv::Scalar(0), inverseMask);
    
    
    workingHandImage = roiFrame(rect);
//    imshow("removeNoiseOutside", workingHandImage);
    return workingHandImage;
    
}

vector <Point> fingerTips;

float distanceP2P(Point a, Point b){
    float d= sqrt(fabs( pow(a.x-b.x,2) + pow(a.y-b.y,2) )) ;
    return d;
}


int getAngleABC( Point a, Point b, Point c ) {
    Point ab = { b.x - a.x, b.y - a.y };
    Point cb = { b.x - c.x, b.y - c.y };
    
    float dot = (ab.x * cb.x + ab.y * cb.y); // dot product
    float cross = (ab.x * cb.y - ab.y * cb.x); // cross product
    
    float alpha = atan2(cross, dot);
    
    return (int) floor(alpha * 180. / PI + 0.5);
}

void getFingerTips(Mat m, vector<Vec4i>  defects, vector<Point> biggestContour, Rect rect){
    vector<Vec4i>::iterator d;
    
    fingerTips.clear();
    int i=0;
    d = defects.begin();
    while( defects.size() > i && d!=defects.end() ) {
        int startIdx = defects[i].val[0];
        Point ptStart(biggestContour[startIdx]);
        
        int endIdx = defects[i].val[1];
        Point ptEnd(biggestContour[endIdx]);
        
        int farIdx = defects[i].val[2];
        Point ptFar(biggestContour[farIdx]);
        
        //        printf("defects: %d %d %d %d %d %d\n", ptStart.x, ptStart.y, ptEnd.x, ptEnd.y, ptFar.x, ptFar.y);
        //        if(i == 0){
        //            fingerTips.push_back(ptStart);
        //            i++;
        //        }
        fingerTips.push_back(ptEnd);
        d++;
        i++;
    }
    //    printf("fingerTips: %lu \n", fingerTips.size());
    //    if(fingerTips.size()==0){
    //        checkForOneFinger(m);
    //    }
}

bool detectIfHand(Mat image, Rect rect){
    double h = rect.height;
    double w = rect.width;
    bool isHand=true;
    if(fingerTips.size() > 5 ){
        isHand=false;
    }else if(h < 100 || w < 100){
        isHand=false;
    }
    else if(h/w > 4 || w/h >4){
        isHand=false;
    }
    return isHand;
}

string intToString(int number){
    stringstream ss;
    ss << number;
    string str = ss.str();
    return str;
}

void drawFingerTips(Mat workingImage) {
    Point p;
    printf("NUMBER OF FINGERS: %lu\n", fingerTips.size());
    for(int i = 0;i < fingerTips.size(); i++){
        p = fingerTips[i];
        putText(workingImage,intToString(i),p-Point(0,30),1, 1.2f,Scalar(200,200,200),2);
        circle(workingImage,p, 5, Scalar(100,255,100), 4 );
        imshow("result", workingImage);
    }
}

float innerAngle(float px1, float py1, float px2, float py2, float cx1, float cy1) {
    
    float dist1 = std::sqrt(  (px1-cx1)*(px1-cx1) + (py1-cy1)*(py1-cy1) );
    float dist2 = std::sqrt(  (px2-cx1)*(px2-cx1) + (py2-cy1)*(py2-cy1) );
    
    float Ax, Ay;
    float Bx, By;
    float Cx, Cy;
    
    //find closest point to C
    //printf("dist = %lf %lf\n", dist1, dist2);
    
    Cx = cx1;
    Cy = cy1;
    if(dist1 < dist2)
    {
        Bx = px1;
        By = py1;
        Ax = px2;
        Ay = py2;
        
        
    }else{
        Bx = px2;
        By = py2;
        Ax = px1;
        Ay = py1;
    }
    
    
    float Q1 = Cx - Ax;
    float Q2 = Cy - Ay;
    float P1 = Bx - Ax;
    float P2 = By - Ay;
    
    
    float A = std::acos( (P1*Q1 + P2*Q2) / ( std::sqrt(P1*P1+P2*P2) * std::sqrt(Q1*Q1+Q2*Q2) ) );
    
    A = A*180/CV_PI;
    
    return A;
}

void searchForDTCenter(Mat image) {
    handDetectedCenterPoint = Point(0, 0);
    int nrOfMaxPoints = 0;
    float maximum = -1;

    
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

    circle(image, handDetectedCenterPoint, 5, Scalar(100,255,100), 3);
    Mat colorImage = coloredRoiFrame.clone();
    circle(colorImage, handDetectedCenterPoint, 5, Scalar(100,255,100), 3);
    imshow("DT center", colorImage);
}

int displayHistogramArray(int* histogram, String windowName, int length){
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
            if (histogram[i] != histogram[i + 1] && histogram[i + 1] == 0) {
                started = true;
            }
            if (histogram[i] != histogram[i + 1] && histogram[i + 1] != 0) {
                started = false;
//                cout<<"nrOfContinuousHand: " << nrOfContinuousHand << "\n";
                if (nrOfContinuousHand < 30.0 && nrOfContinuousHand > 0) {
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
    imshow(windowName, histogramImage);
    return nrOfFingers;
}

int findNumberOfFingers(Mat image, int radius) {
    Size axes (radius,radius);
    vector<Point> pointsOnCircle;
    
    

    
    Mat circleImage = image.clone();
    circle(circleImage, handDetectedCenterPoint, radius, Scalar(100,255,100), 1);
    

    
    
    ellipse2Poly(handDetectedCenterPoint, axes, 0, 0, 360, 1, pointsOnCircle);
    int histogramArray[pointsOnCircle.size()];
    for (int i = 0; i < pointsOnCircle.size(); ++i) {
        
        Point currentPoint = pointsOnCircle[i];
        if (i == 0) {
            circle(circleImage, currentPoint, 10, Scalar(100,255,100), 1);
        }
        if (i == 20) {
            circle(circleImage, currentPoint, 20, Scalar(100,255,100), 1);
        }

        histogramArray[i] = -1.0;
        if (currentPoint.y < image.rows && currentPoint.x < image.cols && currentPoint.x > 0 && currentPoint.y > 0) {
            int colour = image.at<uchar>(currentPoint);
            histogramArray[i] = colour;
        }
    }
    imshow("createdCircle", circleImage);

return    displayHistogramArray(histogramArray, "histogramArray",(int)pointsOnCircle.size());
//    int nrOfFingers = 0;
//    for (int i = 0; i < pointsOnCircle.size() - 1; ++i) {
//        if (histogramArray[i] != histogramArray[i + 1] && histogramArray[i + 1] != 0) {
//            nrOfFingers++;
//        }
//    }
    
//    return nrOfFingers;
}

void findOuterCircle(Mat image) {
    float width = handDetectedRect.width;
    int radius = width / 2.0;
    int maxRadius = radius;
//    cout<< "nrOfFingers: ";
    for (;radius <= maxRadius; ++radius) {
        int nrOfFingers = findNumberOfFingers(image, radius);
        cout<< " " << nrOfFingers << " ";
    }
    cout<< "\n";
}

void computeHandImage() {
    GaussianBlur(roiFrame, roiFrame, Size(0, 0), 2);
    
    Mat workingImage = roiFrame.clone();
    
    vector<vector<Point> > contours;
    Mat_<float> kernel2(3,3);
    kernel2 <<   1, 1, 1,
    1, 1, 1,
    1, 1, 1;
    Mat_<float> kernel(5,5);
    kernel <<   1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1;
    
    
    morphologyEx(workingImage, workingImage, MORPH_OPEN, kernel);
    morphologyEx(workingImage, workingImage, MORPH_CLOSE, kernel);
    distanceTranformImage = workingImage.clone();
    
    //find the biggest contour
    findContours(workingImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    
    int indexOfBiggestContour = findBiggestContour(contours);
    if (indexOfBiggestContour != -1) {
        vector<Point> biggestContour = contours[indexOfBiggestContour];
        handDetectedRect = boundingRect(biggestContour);
        rectangle(workingImage, Point(handDetectedRect.x, handDetectedRect.y), Point(handDetectedRect.x + handDetectedRect.width, handDetectedRect.y + handDetectedRect.height), Scalar(255.0, 255.0, 0));
        rectangle(coloredRoiFrame, Point(handDetectedRect.x, handDetectedRect.y), Point(handDetectedRect.x + handDetectedRect.width, handDetectedRect.y + handDetectedRect.height), Scalar(255.0, 255.0, 0));
        
        Mat workingImageWithoutNoise = removeNoiseOutside(workingImage, handDetectedRect);
        //        imshow("image after removeNoiseOutside", workingImageWithoutNoise);
        
        Mat dist;
        distanceTransform(workingImage.clone(), dist, CV_DIST_L2, 3);
        
        // Normalize the distance image for range = {0.0, 1.0}
        // so we can visualize and threshold it
        normalize(dist, dist, 0, 1., NORM_MINMAX);
        
        vector<Point> hullPoints;
        vector<int> hullI;
        
        convexHull(Mat(biggestContour), hullPoints, false, true);
        convexHull(Mat(biggestContour), hullI, false, false);
        approxPolyDP(Mat(hullPoints), hullPoints, 1, true);
        
        Mat drawing = workingImage.clone();
        
        Scalar color = Scalar( 255.0, 25.0, 10.0);
        
        vector<vector<Point>> vector1;
        vector1.push_back(hullPoints);
        
        vector<vector<Point>> vector2;
        vector2.push_back(biggestContour);
        
        Mat filledContours = drawing.clone();
        //        drawContours(drawing, vector1, 0, color, 1);
        
        drawContours(drawing, vector2, 0, color, 1, LINE_4);
        drawContours(filledContours, vector2, 0, color, -1);
        //        drawContours(coloredRoiFrame, vector1, 0, color, 1);
        //        drawContours(coloredRoiFrame, vector2, 0, color2, 1);
        imshow("Contours", drawing);
        imshow("Filled Contours", filledContours);
        erode(filledContours, filledContours, kernel);
        imshow("Filled Contours eroded", filledContours);
        
//        filledContours = filledContours(handDetectedRect);
//        imshow("filledContours aci", filledContours);

        Mat DTFilled;
        distanceTransform(filledContours, DTFilled, CV_DIST_L2, 3);
        normalize(DTFilled, DTFilled, 0, 1., NORM_MINMAX);
        imshow("Distance transform filled", DTFilled);
//        DTFilled = DTFilled(handDetectedRect);
        imshow("imageRect", DTFilled);
        
        searchForDTCenter(DTFilled);
        findOuterCircle(filledContours);
        
    }
    roiFrame = workingImage;
}


Mat colorBalance(Mat img) {
    Mat YC, mask;
    
    cvtColor(img, YC, CV_BGR2RGB);
    float avgR = 0.0;
    float avgG = 0.0;
    float avgB = 0.0;
    
    float avgGray = 0.0;
    
    for (int i = 0; i < YC.cols; ++i) {
        for (int j = 0; j < YC.rows; ++j) {
            Vec3b point = YC.at<Vec3b>(Point(i, j));
            avgR += point[0];
            avgG += point[1];
            avgB += point[2];
        }
    }
    avgR /= YC.rows + YC.cols;
    avgG /= YC.rows + YC.cols;
    avgB /= YC.rows + YC.cols;
    
    avgGray = (avgR + avgG + avgB) / 3.0;
    
    float aR = avgGray / avgR;
    float aG = avgGray / avgG;
    float aB = avgGray / avgB;
    Mat YC2;
    
    YC2 = Mat(YC.rows, YC.cols, YC.type());
    
    for (int i = 0; i < YC.cols; ++i) {
        for (int j = 0; j < YC.rows; ++j) {
            Vec3b point = YC.at<Vec3b>(Point(i, j));
            float R = aR * point[0];
            float G = aG * point[1];
            float B = aB * point[2];
            if (R > 255) {
                R = 255.0;
            }
            if (G > 255) {
                G = 255.0;
            }
            if (B > 255) {
                B = 255.0;
            }
            img.at<Vec3b>(Point(i, j)) = Vec3b(R, G, B);
        }
    }
    cvtColor(img, YC, CV_RGB2BGR);

    
    imshow("image after color balance", img);
    return img;
}

void startCamera(){
    VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
//    cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));

    if (!cap.isOpened()) { // openenig the video device failed
        printf("Cannot open video capture device.\n");
        return;
    }
    
    Mat frame;
    
    // video resolution
    capS = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
                (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    
    // Display window
    //    const char* WIN_SRC = "Camera"; //window for the source frame
    //    namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
    //    moveWindow(WIN_SRC, 0, 0);
    
    //    const char* WIN_DST = "Snapped"; //window for showing the snapped frame
    //    namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
    //    moveWindow(WIN_DST, capS.width + 10, 0);
    
    char c;
    int frameNum = -1;
    int frameCount = 0;
    state = 1;
   
    for (;;){
//        double fps = cap.get(CV_CAP_PROP_FPS);
//        cout<<"FPS: "<< fps<<"\n";

        cap >> frame; // get a new frame from camera
        if (frame.empty()) {
            printf("End of the video file\n");
            continue;
        }
        flip(frame, frame, 1);
        imshow("Original", frame);
//        frame = colorBalance(frame);
        ++frameNum;
        //        imshow(WIN_SRC, frame);
        createROI(frame);
//        cout<<state;
        if (state == 1) {
            placeSquares();
        }
        c = waitKey(10);
        
        
        
        if (c == 27) {
            // press ESC to exit
            printf("ESC pressed - capture finished");
            waitKey();
            //            break;  //ESC pressed
        }
        if (c == 115) { //'s' pressed - snapp the image to a file
            state = 2;
            frameCount++;
        }
        
        
        
        if (state == 2) {
            computeMedianHandColor();
            if (currentSample == NR_OF_SAMPLES) {
                state = 3;
            }
            currentSample++;
        }
        //        pyrDown(roiFrame, roiFrame);
        
        if (state >= 3) {
            //            solveHandDetection(roiFrame);
            initTrackbars();
            performBinarization();
            
            state = 4;
        }
        //        pyrUp(roiFrame, roiFrame);
        
        imshow(ROI_NAME, roiFrame);
        if (state >= 4) {
            computeHandImage();
            state = 5;
        }
        
        
    }
}
