//
//  Project.cpp
//  IP2017
//
//  Created by Vlad Bonta on 10/04/2017.
//  Copyright © 2017 Vlad Bonta. All rights reserved.
//

#include "Project.hpp"
#include "Square.hpp"

#define NR_OF_SAMPLES 10
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
Mat coloredRoiFrame;
Mat distanceTranformImage;
void solveHandDetection(Mat img);
void findHand(Mat src);
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
    cvtColor(roiFrame, roiFrame, CV_BGR2HLS);
   
    standardDeviationScalar = Scalar(0.0, 0.0, 0.0);
    for (int squareIndex = 0; squareIndex < squarePoints.size(); ++squareIndex) {
        Square currentSquare = squarePoints[squareIndex];
        Point upperLeftCorner = currentSquare.upperLeftCorner;
        Point lowerLeftCorner = currentSquare.lowerLeftCorner;
        
        Rect sqaureRect = Rect(upperLeftCorner.x, upperLeftCorner.y, currentSquare.squareSize, currentSquare.squareSize);
        Mat squareImage = roiFrame(sqaureRect);
        
        Scalar mean,dev;
        
        meanStdDev(squareImage, mean, dev);
        standardDeviationScalar[0] += mean[0];
        standardDeviationScalar[1] += mean[1];
        standardDeviationScalar[2] += mean[2];
        
//        standardDeviationScalar[0] += dev[0];
//        standardDeviationScalar[1] += dev[1];
//        standardDeviationScalar[2] += dev[2];
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
    
    cvtColor(roiFrame, roiFrame, CV_HLS2BGR);
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
    
    cvtColor(roiFrame, roiFrame, CV_BGR2HLS);
    imshow("HLS", roiFrame);
    vector<Mat> bwFrameList;
    
    float k = 0.55;
    for (int i = 0; i < NR_OF_SAMPLES;++i) {


        lowerBoundColor[0] = medianColor[i][0] - k*standardDeviationScalar[0];
        lowerBoundColor[1] = medianColor[i][1] - k*standardDeviationScalar[1];
        lowerBoundColor[2] = medianColor[i][2] - k*standardDeviationScalar[2];

        upperBoundColor[0] = medianColor[i][0] + k*standardDeviationScalar[0];
        upperBoundColor[1] = medianColor[i][1] + k*standardDeviationScalar[1];
        upperBoundColor[2] = medianColor[i][2] + k*standardDeviationScalar[2];

        Mat bwFrame;
        inRange(roiFrame.clone(), lowerBoundColor, upperBoundColor, bwFrame);
        bwFrameList.push_back(bwFrame);
    }

    finalBWFrame = bwFrameList[0];
    
    for (int i = 1; i < NR_OF_SAMPLES;++i) {
        finalBWFrame += bwFrameList[i];
    }
    
    medianBlur(finalBWFrame, roiFrame, 7);
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
    imshow("removeNoiseOutside", workingHandImage);
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
//    imshow("222", workingImage);
        morphologyEx(workingImage, workingImage, MORPH_CLOSE, kernel);
//    imshow("33", workingImage);
    distanceTranformImage = workingImage.clone();

    //find the biggest contour
    findContours(workingImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    

    int indexOfBiggestContour = findBiggestContour(contours);
    if (indexOfBiggestContour != -1) {
        vector<Point> biggestContour = contours[indexOfBiggestContour];
        Rect rect = boundingRect(biggestContour);
        rectangle(workingImage, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(255.0, 255.0, 0));
        rectangle(coloredRoiFrame, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(255.0, 255.0, 0));
        
        
        removeNoiseOutside(workingImage, rect);
        imshow("image before DT", workingImage);
        Mat dist;
        distanceTransform(workingImage.clone(), dist, CV_DIST_L2, 3);
        
        // Normalize the distance image for range = {0.0, 1.0}
        // so we can visualize and threshold it
        normalize(dist, dist, 0, 1., NORM_MINMAX);
//        imshow("Distance Transform Image", dist);
        
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

        
        Mat dist2;
        distanceTransform(filledContours, dist2, CV_DIST_L1, 3);
        // Normalize the distance image for range = {0.0, 1.0}
        // so we can visualize and threshold it
        normalize(dist2, dist2, 0, 1., NORM_MINMAX);
        imshow("Distance transform filled", dist2);
        
        Mat defectsImage = coloredRoiFrame.clone();
        Mat defectsImage2 = coloredRoiFrame.clone();
        
        /*
        vector<Vec4i>  defects;
        if (biggestContour.size() > 3) {
            convexityDefects(biggestContour, hullI, defects);
            

            vector<Vec4i> goodDefects;
            int tolerance =  rect.height / 5;
            
            Point center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
            
            
            
            
            
            int j = 1;
            for (int i = 0; i < defects.size() && j < defects.size(); i++, j++) {
                int startIdx = defects[i].val[0];
                Point ptStart(biggestContour[startIdx]);
                
                int endIdx = defects[i].val[1];
                Point ptEnd(biggestContour[endIdx]);
                
                int farIdx = defects[i].val[2];
                Point ptFar(biggestContour[farIdx]);
                
                int startIdx2 = defects[j].val[0];
                Point ptStart2(biggestContour[startIdx2]);
                
                int endIdx2 = defects[j].val[1];
                Point ptEnd2(biggestContour[endIdx2]);
                
                int farIdx2 = defects[j].val[2];
                Point ptFar2(biggestContour[farIdx2]);
                
                float distanceBetweenMidleAndFarPoint = distanceP2P(ptStart, ptStart2);
                float distanceBetweenMidleAndFarPoint2 = distanceP2P(ptEnd, ptEnd2);
                if (distanceBetweenMidleAndFarPoint < rect.width / 6 && distanceBetweenMidleAndFarPoint2 < rect.width / 6) {
                    
                    //                    goodDefects.push_back(defects[i]);
                }
            }
            
            for (int i = 0; i < defects.size(); i++) {
                for (int j = i + 1; j < defects.size(); j++) {
                    int startIdx = defects[i].val[0];
                    Point ptStart(biggestContour[startIdx]);
                    
                    int startIdx2 = defects[j].val[0];
                    Point ptStart2(biggestContour[startIdx2]);
                    
                    float distance1 = distanceP2P(ptStart, center);
                    float distance2 = distanceP2P(ptStart2, center);
                    
                    if (distance1 < distance2) {
                        Vec4i aux = defects[i];
                        defects[i] = defects[j];
                        defects[j] = aux;
                    }
                }
            }
            for (int i = 0; i < 10 && i < defects.size(); i++) {
                //                goodDefects.push_back(defects[i]);
                
            }
            
            for (int i = 0; i < defects.size(); i++) {
                
                int startIdx = defects[i].val[0];
                Point ptStart(biggestContour[startIdx]);
                
                int endIdx = defects[i].val[1];
                Point ptEnd(biggestContour[endIdx]);
                
                int farIdx = defects[i].val[2];
                Point ptFar(biggestContour[farIdx]);
                
                
                circle(defectsImage2,ptStart,5,CV_RGB(255,0,0),2,8);
                
                circle(defectsImage2,ptFar,5,CV_RGB(0,0,255),2,8);
                
                circle(defectsImage2,ptEnd,5,CV_RGB(0,255,0),2,8);
                
                float angleTol = 95;
                
                double inAngle = innerAngle(ptStart.x, ptStart.y, ptFar.x, ptFar.y, ptEnd.x, ptEnd.y);
                Point middlePoint = Point((ptStart.x + ptEnd.x) / 2.0, (ptStart.y + ptEnd.y) / 2.0);
                float distanceBetweenMidleAndFarPoint = distanceP2P(middlePoint, ptFar);
                //                printf("distanceBetweenMidleAndFarPoint: %f\n", distanceBetweenMidleAndFarPoint);
                
                
                
                //                if(distanceP2P(ptStart, ptFar) > tolerance && distanceP2P(ptEnd, ptFar) > tolerance){
                
                if(distanceP2P(ptEnd, ptFar) > tolerance){
                    //                    goodDefects.push_back(defects[i]);
                    if (ptEnd.y < rect.y + rect.height - 2.0) {
                        if (getAngleABC(ptStart, ptFar, ptEnd) < angleTol) {
                            if (distanceBetweenMidleAndFarPoint > 0.5) {
                                goodDefects.push_back(defects[i]);
                            }
                            
                            if( ptEnd.y > (rect.y + rect.height -rect.height/4 ) ){
                            } else if( ptStart.y > (rect.y + rect.height -rect.height/4 ) ){
                            } else {
                                //                            goodDefects.push_back(defects[i]);
                            }
                        }
                    }
                }
            }
            
            
            //            Vec4i temp;
            //            float avgX, avgY;
            //            tolerance=rect.width /6;
            //            int startidx, endidx, faridx;
            //            int startidx2, endidx2;
            //            for(int i=0;i<goodDefects.size();i++){
            //                for(int j=i;j<goodDefects.size();j++){
            //                    startidx=goodDefects[i][0];
            //                    Point ptStart(biggestContour[startidx] );
            //                    endidx=goodDefects[i][1];
            //                    Point ptEnd(biggestContour[endidx] );
            //                    startidx2=goodDefects[j][0];
            //                    Point ptStart2(biggestContour[startidx2] );
            //                    endidx2=goodDefects[j][1];
            //                    Point ptEnd2(biggestContour[endidx2] );
            //                    if(distanceP2P(ptStart,ptEnd2) < tolerance ){
            //                        biggestContour[startidx] = ptEnd2;
            //                        break;
            //                    }if(distanceP2P(ptEnd,ptStart2) < tolerance ){
            //                        biggestContour[startidx2]=ptEnd;
            //                    }
            //                }
            //            }
            
            
            
            
            
            for (int i = 0; i < goodDefects.size(); i++) {
                int startIdx = goodDefects[i].val[0];
                Point ptStart(biggestContour[startIdx]);
                
                int endIdx = goodDefects[i].val[1];
                Point ptEnd(biggestContour[endIdx]);
                
                int farIdx = goodDefects[i].val[2];
                Point ptFar(biggestContour[farIdx]);
                
                circle(defectsImage,ptStart,5,CV_RGB(255,0,0),2,8);
                circle(defectsImage,ptFar,5,CV_RGB(0,0,255),2,8);
                
                circle(defectsImage,ptEnd,5,CV_RGB(0,255,0),2,8);
            }
            //            imshow("Defects", defectsImage);
            //            imshow("DefectsAll", defectsImage2);
            //            printf("defects: %lu \n", goodDefects.size());
            
            //            getFingerTips(workingImage, goodDefects, biggestContour, rect);
            
            bool isHand = detectIfHand(workingImage, rect);
            if (isHand) {
                drawFingerTips(workingImage);
            }
        }
         */
    }
//    findHand(workingImage);
    roiFrame = workingImage;
}

void findHand(Mat src) {
    // Check if everything was fine
    if (!src.data)
        return ;
    // Show source image
    imshow("Source Image", src);
    // Create a kernel that we will use for accuting/sharpening our image
    //    Mat kernel = (Mat_<float>(3,3) <<
    //                  1,  1, 1,
    //                  1, -8, 1,
    //                  1,  1, 1); // an approximation of second derivative, a quite strong kernel
    //
    //    Mat imgLaplacian;
    //    Mat sharp = src; // copy source image to another temporary one
    //    filter2D(sharp, imgLaplacian, CV_32F, kernel);
    //    src.convertTo(sharp, CV_32F);
    //    Mat imgResult = sharp - imgLaplacian;
    //    // convert back to 8bits gray scale
    //    imgResult.convertTo(imgResult, CV_8UC3);
    //    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    //    // imshow( "Laplace Filtered Image", imgLaplacian );
    //    imshow( "New Sharped Image", imgResult );
    //    src = imgResult; // copy back
    //    // Create binary image from source image
    //    Mat bw;
    //    cvtColor(src, bw, CV_BGR2GRAY);
    //    threshold(bw, bw, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //    imshow("Binary Image", bw);
    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(src, dist, CV_DIST_L1, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1., NORM_MINMAX);
    imshow("Distance ", distanceTranformImage);
    imshow("Distance Transform Image2", dist);
    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
//    threshold(dist, dist,127,255,CV_THRESH_BINARY);
    
    //    threshold(dist, dist, .8, 1., CV_THRESH_BINARY);
}

void solveHandDetection(Mat img) {
    Mat YC, mask;
    
    cvtColor(img, YC, CV_BGR2RGB);
    float avgR = 0.0;
    float avgG = 0.0;
    float avgB = 0.0;
  
    float avgGray = 0.0;
    
    for (int i = 0; i < YC.rows; ++i) {
        for (int j = 0; j < YC.cols; ++j) {
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

    for (int i = 0; i < YC.rows; ++i) {
        for (int j = 0; j < YC.cols; ++j) {
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
    
   
    
    imshow("image after color balance", img);

//    cvtColor(img, img, CV_BGR2YCrCb);
//    imshow("image after color balance YCrCb", img);

    mask = Mat(img.rows, img.cols, CV_8U);
//    imshow("YC", YC);
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            Vec3b point = img.at<Vec3b>(Point(i, j));
            float Y = 16.0 + point[0] * 65.481 + 128.553 * point[1] + 24.966 * point[2];
            float Cg = 128.0 + point[0] * -81.085 + 112.0 * point[1] + -30.915 * point[2];
            float Cr = 128.0 + point[0] * 112.0 + -93.786 * point[1] + -18.214 * point[2];
            
//            float Cg = 128.0 + point[1] * 146.767;
//            float Cr = 128.0 + point[2] * -24.163;
//            float Cg = 162.879 * point[0];
//            float Cr = 103.837 * point[2];
            img.at<Vec3b>(Point(i, j)) = Vec3b(Y, Cg, Cr);
            if (Cg > 85 && Cg< 135 && Cr > -Cg + 260 && Cr < -Cg + 280) {
//                mask.at<uchar>(Point(i, j)) =  255.0;
            } else {
//                mask.at<uchar>(Point(i, j)) =  0.0;

            }
        }
    }
    imshow("lastFrame", YC);

    imshow("MASKSSS", mask);
}

void startCamera(){
    VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
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
        cap >> frame; // get a new frame from camera
        if (frame.empty()) {
            printf("End of the video file\n");
            continue;
        }
        flip(frame, frame, 1);
        ++frameNum;
        //        imshow(WIN_SRC, frame);
        createROI(frame);
        cout<<state;
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
