#pragma once

#include "ofMain.h"
#include "ofGraphics.h"
#include "ofxOpenCv.h"

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
    
    //VideoInput
    ofVideoGrabber         vidGrabber;
    
    ofxCvColorImage            colorImg;
    ofxCvGrayscaleImage     grayImage;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     grayDiff;
    
    ofxCvContourFinder     contourFinder;
    
    int                 threshold;
    bool                bLearnBakground;
    
    //AudioOutput
    float const S_TRIGGER = 0.005;
    void checkSound();
    
    void move();
    
    //VideoOutput
    ofRectangle blobRect;
    
    void jump();
    void duck();
    
    //Background
    ofImage         background;
    
    //Animation
    ofImage character;
    float xPos, yPos, startingX, startingY, endX, endY, endJump, endDuck, rectX, rectY;
    void movetoStart();
    
    //block
    ofImage block;
    float movingX = 570;
    
    //shoot
    int bulletX = 30;
    void shoot();
    
    //UI
    int grayDiffX = 660;
    int grayDiffY = 130;
    
    int backgroundWidth = 640;
    int backgroundHeight = 480;
    
    int jumpTrigger = grayDiffY+70;
    int duckTrigger = grayDiffY+140;
    
    
    
};

