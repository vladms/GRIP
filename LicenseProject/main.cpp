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
#include "ImageRepository.hpp"
#include "Level.hpp"

using namespace cv;
using namespace std;

void showLevelsMenu();
void showLevelCompletionMenu();
void startLevel(int level);
void stopLevel();
void levelFinishedSuccesfully();

int menuState;
Mat canvas;
string levelsWindowName = "Alege nivelul";
string levelCompletionWindowName = "Felicitari";

Rect level1Button;
Rect level2Button;
Rect level3Button;
Rect level4Button;

Rect instructionsButton;

Rect levelFinishedCloseButton;
Rect levelFinishedNextButton;


Level *currentLevel;
int levelNumber;
bool sPressed;
VideoCapture cap(0);
Size cameraSize;
Size menuSize;


void callBackFunc(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        cout<< x<< " " << y << endl;
        if (menuState == 1) {
            if (level1Button.contains(Point(x, y))) {
                destroyWindow(levelsWindowName);
                levelNumber = 1;
                startLevel(levelNumber);
            }
            if (level2Button.contains(Point(x, y))) {
                destroyWindow(levelsWindowName);
                levelNumber = 2;
                startLevel(levelNumber);
            }
            if (level3Button.contains(Point(x, y))) {
                destroyWindow(levelsWindowName);
                levelNumber = 3;
                startLevel(levelNumber);
            }
            if (level4Button.contains(Point(x, y))) {
                destroyWindow(levelsWindowName);
                levelNumber = 4;
                startLevel(levelNumber);
            }
            
            if (instructionsButton.contains(Point(x, y))) {
                destroyWindow(levelsWindowName);
                //Display instructions
            }
        } else if (menuState == 2) {
            
            if (levelFinishedCloseButton.contains(Point(x, y))) {
                destroyWindow(levelCompletionWindowName);
                destroyWindow("GRIP");
                showLevelsMenu();
            }
            
            if (levelFinishedNextButton.contains(Point(x, y))) {
                destroyWindow(levelCompletionWindowName);
                levelNumber++;
                startLevel(levelNumber);
            }
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
    
    Mat img(menuSize.width, menuSize.height, CV_8UC3, Scalar(0, 0, 0));
    int buttonHeight = 100;
    int buttonDistance = 20;
    level1Button = Rect(0, 0, menuSize.width, buttonHeight);
    level2Button = Rect(0, buttonHeight + buttonDistance, menuSize.width, buttonHeight);
    level3Button = Rect(0, 2 * (buttonHeight + buttonDistance), menuSize.width, buttonHeight);
    level4Button = Rect(0, 3 * (buttonHeight + buttonDistance), menuSize.width, buttonHeight);
    instructionsButton = Rect(0, 4 * (buttonHeight + buttonDistance), menuSize.width, buttonHeight);
    
    // The canvas
    canvas = Mat(img.cols, img.rows, CV_8UC3, Scalar(255.0, 255.0, 255.0));
    
    Mat menuBackgroundImage = loadMenuBackground();
    resize(menuBackgroundImage, menuBackgroundImage, canvas.size());
    menuBackgroundImage.copyTo(canvas);
    
    
    Mat buttonBackgroundImage;
    buttonBackgroundImage = loadMenuButtonsBackground();
    
    resize(buttonBackgroundImage, buttonBackgroundImage, level1Button.size());
    buttonBackgroundImage.copyTo(canvas(level1Button));
    buttonBackgroundImage.copyTo(canvas(level2Button));
    buttonBackgroundImage.copyTo(canvas(level3Button));
    buttonBackgroundImage.copyTo(canvas(level4Button));
    buttonBackgroundImage.copyTo(canvas(instructionsButton));
    
    putText(canvas(level1Button), "ADUNARE MERE", Point(2 * level1Button.width / 5.5, level1Button.height*0.5), FONT_HERSHEY_PLAIN, 3, Scalar(0,0,0), 5);
    putText(canvas(level2Button), "SCADERE LAMAI", Point(2 * level2Button.width / 5.5, level2Button.height*0.5), FONT_HERSHEY_PLAIN, 3, Scalar(0,0,0), 5);
    putText(canvas(level3Button), "ADUNARE NUMERE", Point(2 * level3Button.width / 5.5, level3Button.height*0.5), FONT_HERSHEY_PLAIN, 3, Scalar(0,0,0), 5);
    putText(canvas(level4Button), "SCADERE NUMERE", Point(2 * level4Button.width / 5.5, level4Button.height*0.5), FONT_HERSHEY_PLAIN, 3, Scalar(0,0,0), 5);
    putText(canvas(instructionsButton), "INSTRUCTIUNI", Point(2 * instructionsButton.width / 5.5, instructionsButton.height*0.5), FONT_HERSHEY_PLAIN, 3, Scalar(0,0,0), 5);
    
    // Setup callback function
    namedWindow(levelsWindowName);
    setMouseCallback(levelsWindowName, callBackFunc);
    
    imshow(levelsWindowName, canvas);
    moveWindow(levelsWindowName, 0, 0);
    waitKey();
}

void showLevelCompletionMenu() {
    menuState = 2;
    
    Mat img(menuSize.width, menuSize.height, CV_8UC3, Scalar(0, 0, 0));
    int smallButtonHeight = 50;
    int smallButtonWidth = 80;
    
    int buttonHeight = 80;
    int buttonWidth = 500;
    int buttonDistance = 60;
    
    
    levelFinishedCloseButton = Rect(0, 0, smallButtonWidth, smallButtonHeight);
    
    
    Rect levelFinishedScoreRectButton = Rect((menuSize.width - buttonWidth) / 2.0, buttonDistance + (menuSize.height / 2 - buttonHeight) / 2.0, buttonWidth, 2 * buttonHeight);
    
    levelFinishedNextButton = Rect((menuSize.width - buttonWidth) / 2.0, levelFinishedScoreRectButton.y + levelFinishedScoreRectButton.height + buttonDistance, buttonWidth, buttonHeight);
    
    Rect levelFinishedCongratsRectButton = Rect((menuSize.width - buttonWidth) / 2.0, (menuSize.height / 4 - buttonHeight) / 2.0, buttonWidth, buttonHeight);
    
    
    // The canvas
    canvas = Mat(img.cols, img.rows, CV_8UC3, Scalar(255.0, 255.0, 255.0));
    
    Mat menuBackgroundImage = loadMenuBackground();
    resize(menuBackgroundImage, menuBackgroundImage, canvas.size());
    menuBackgroundImage.copyTo(canvas);
    
    
    Mat buttonBackgroundImage;
    buttonBackgroundImage = loadMenuButtonsBackground();
    
    resize(buttonBackgroundImage, buttonBackgroundImage, levelFinishedCloseButton.size());
    buttonBackgroundImage.copyTo(canvas(levelFinishedCloseButton));
    
    resize(buttonBackgroundImage, buttonBackgroundImage, levelFinishedNextButton.size());
    buttonBackgroundImage.copyTo(canvas(levelFinishedNextButton));
    
    resize(buttonBackgroundImage, buttonBackgroundImage, levelFinishedCongratsRectButton.size());
    buttonBackgroundImage.copyTo(canvas(levelFinishedCongratsRectButton));
    
    resize(buttonBackgroundImage, buttonBackgroundImage, levelFinishedScoreRectButton.size());
    buttonBackgroundImage.copyTo(canvas(levelFinishedScoreRectButton));
    
    putText(canvas(levelFinishedCongratsRectButton), "FELICITARI !!!", Point(0.0, levelFinishedCongratsRectButton.height), FONT_HERSHEY_PLAIN, 5, Scalar(0,0,0), 5);
    
    if (!currentLevel) {
        currentLevel = new Level;
        currentLevel->score = 5;
        currentLevel->numberOfSublevels = 5;
    }
    
    string scoreString = "Scorul tau: " ;
    string scoreValueString = to_string(currentLevel->score) + " din " + to_string(currentLevel->numberOfSublevels);
    
    putText(canvas(levelFinishedScoreRectButton), scoreString, Point((levelFinishedScoreRectButton.width - 100) / 4.0, levelFinishedScoreRectButton.height * 0.3), FONT_HERSHEY_PLAIN, 4, Scalar(0,0,0), 4);
    putText(canvas(levelFinishedScoreRectButton), scoreValueString, Point((levelFinishedScoreRectButton.width - 100) / 4.0, levelFinishedScoreRectButton.height * 0.3 + 100), FONT_HERSHEY_PLAIN, 4, Scalar(0,0,0), 4);
    
    putText(canvas(levelFinishedCloseButton), "INCHIDE", Point(0, levelFinishedCloseButton.height*0.5), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0), 1);
    putText(canvas(levelFinishedNextButton), "URMATORUL NIVEL", Point(0.0, levelFinishedNextButton.height*0.5), FONT_HERSHEY_PLAIN, 3, Scalar(0,0,0), 5);
    
    // Setup callback function
    namedWindow(levelCompletionWindowName);
    setMouseCallback(levelCompletionWindowName, callBackFunc);
    
    imshow(levelCompletionWindowName, canvas);
    moveWindow(levelCompletionWindowName, 0, 0);
    waitKey();
}

void levelFinishedSuccesfully() {
    //Display a congratiulations screen
    destroyWindow("GRIP");
    showLevelCompletionMenu();
}

void stopLevel() {
    destroyWindow("GRIP");
    showLevelsMenu();
}


void startLevel(int level) {
    bool playing = true;
    char c;
    sPressed = false;
    currentLevel = new Level;
    currentLevel->initLevel(level);
    
    int state = 1;
    int frameCount = 0;
    
    time_t now = time(0);
    time_t next = time(0);
    
    float secondsPassed = 0;
    
    setupLevel(currentLevel);
    Mat frame;
    cap>>frame;
    if (!cap.isOpened()) {
        printf("Cannot open video capture device.\n");
        return;
    }
    
    while(playing) {
        Mat frame;
        c = waitKey(1);
        
        cap >> frame; // get a new frame from cameras
        
        bool updateResult = false;
        
        if (state > 2) {
            next = time(0);
            secondsPassed = next - now;
        }
        
        if (secondsPassed >= currentLevel->seconds && state > 2) {
            //new sublevel
            if (currentLevel->currentSublevelIndex < currentLevel->numberOfSublevels - 1) {
                currentLevel->currentSublevelIndex++;
                updatePlayingLevel(currentLevel);
            } else {
                currentLevel->currentSublevelIndex++;

                playing = false;
                frameCount = 1;
                now = next;
                updateResult = true;
                state = newFrame(cap, frame, state, true, currentLevel->seconds - secondsPassed);
                
                levelFinishedSuccesfully();
                //                break;
            }
            frameCount = 1;
            now = next;
            updateResult = true;
            state = newFrame(cap, frame, state, updateResult, currentLevel->seconds - secondsPassed);
            
            
        } else {
            state = newFrame(cap, frame, state, updateResult, currentLevel->seconds - secondsPassed);
        }
        
        if (c == 27) {
            // press ESC to exit level
            
            playing = false;
            stopLevel();
            printf("ESC pressed - capture finished");
            
            //waitKey();
            //break;  //ESC pressed
        }
        
        if (c == 115 && !sPressed) {
            state = 2;
            next = time(0);
            now = next;
            sPressed = true;
        }
        
    }
    
}




int main(){
    
//        system("afplay /Users/VladBonta/Desktop/test.mp3");
    cameraSize = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
                      (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    
    menuSize = cameraSize;
    showLevelsMenu();
    //    showLevelCompletionMenu();
    //        startCamera();
    
    
    return 0;
}


