#pragma once

#include "ofMain.h"
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
    
    //Audio
    void audioIn(ofSoundBuffer & input);
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int     bufferCounter;
    int     drawCounter;
    
    
    float smoothedVol;
    float scaledVol;
    
    float curVol;
    float const S_TRIGGER=0.05;
    
    
    ofSoundStream soundStream;
    
    //Video

    ofVideoGrabber         vidGrabber;
    ofxCvGrayscaleImage     grayImage;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     grayDiff;
    ofxCvColorImage            colorImg;
    
    ofxCvContourFinder     contourFinder;
    
    int                 threshold;
    bool                bLearnBakground;
    
    ofRectangle blobRect;
    
    void jump();
    void duck();
    ofRectangle rect;
    
};

