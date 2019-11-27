#pragma once

#include "ofMain.h"
#include "ofGraphics.h"
#include "ofxOpenCv.h"
#include <stdlib.h>  

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
    // AudioInput
    void audioIn(ofSoundBuffer & input);
    
    vector <float> left;
    vector <float> right;
    float smoothedVol;
    float scaledVol;
    ofSoundStream soundStream;
    float curVol;
    
    //AudioOutput
    float const S_TRIGGER = 0.01;
    void checkSound();
    void move();
    
    //VideoInput
    ofVideoGrabber         vidGrabber;
    ofxCvColorImage         colorImg;
    ofxCvGrayscaleImage     grayImage;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     grayDiff;
    ofxCvContourFinder     contourFinder;
    int                 threshold;
    bool                bLearnBakground;
    int                 YPixel;
    
    
    //VideoOutput
    ofRectangle blobRect;
    void jump();
    void duck();
    void movetoStart();
    
    //Background
    ofImage background;
    
    //Animation
    ofImage character;
    int startingX, startingY, endX, endY, endJump, endDuck, charX, charY;
    int generateRandomY();
    void checkForCollisions();
    
    //life
    bool gamestate=false;
    int lifeCounter = 3;
    ofImage heart1;
    ofImage heart2;
    ofImage heart3;
    
    //meteor
    ofImage meteor;
    float meteorX = 570;
    float meteorY = 200;
    
    ofImage meteor2;
    float meteorX2 = 570;
    float meteorY2 = 300;
    
    //UI
    int grayDiffX = 660;
    int grayDiffY = 130;
    
    int backgroundWidth = 640;
    int backgroundHeight = 480;
    
    int jumpTrigger = grayDiffY+70;
    int duckTrigger = grayDiffY+140;
    
    ofVideoPlayer countdown;
    
    //Enemy
    ofImage enemy;
    float enemyY = 150;
    float enemyX = 400;
    void scare();
    
    
};

