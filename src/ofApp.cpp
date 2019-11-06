#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    soundStream.printDeviceList();
    
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    
    bufferCounter    = 0;
    drawCounter        = 0;
    smoothedVol     = 0.0;
    scaledVol        = 0.0;
    
    
    ofSoundStreamSettings settings;
    
    auto devices = soundStream.getMatchingDevices("default");
    if(!devices.empty()){
        settings.setInDevice(devices[0]);
    }
    
    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    //Video
    vidGrabber.setVerbose(true);
    vidGrabber.setup(320,240);

    colorImg.allocate(320,240);
    grayImage.allocate(320,240);
    grayBg.allocate(320,240);
    grayDiff.allocate(320,240);
    
    bLearnBakground = true;
    threshold = 60;
    
    //Background
    frameByframe = false;
    background.load("galaxy.mov");
    background.setLoopState(OF_LOOP_NORMAL);
    background.play();
    
    //Animation
    startingX=100;
    startingY=100;
    endJump=10;
    endDuck=300;
    
    rect.x = startingX;
    rect.y = startingY;
    rect.width = 100;
    rect.height = 100;
    ofSetColor(0,255,0);
    
    ofDrawRectangle(rect);
    
    blobRect.x = startingX;
    blobRect.y = startingY;
}

//--------------------------------------------------------------
void ofApp::update(){
    //lets scale the vol up to a 0-1 range
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    //Video
    ofBackground(100,100,100);
    
    bool bNewFrame = false;
    
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();

    
    if (bNewFrame){
        
  
        colorImg.setFromPixels(vidGrabber.getPixels());
        grayImage = colorImg;
        if (bLearnBakground == true){
            grayBg = grayImage;
            bLearnBakground = false;
        }

        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 3000,30000, 1,false, true);
        
        //Background
        background.update();
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //Background
    background.draw(10,10,640,480);
    
    //Audio
    ofDrawBitmapString(ofToString(curVol,2), 20, 200);
    
    //Video
    // draw the incoming, the grayscale, the bg and the thresholded difference
    ofSetHexColor(0xffffff);
    //colorImg.draw(20,20);
    //grayImage.draw(150,10);
    //grayBg.draw(20,280);
    grayDiff.draw(700,10);
    
    // then draw the contours:
    
    ofFill();
    ofSetHexColor(0x333333);
    //ofDrawRectangle(360,540,320,240);
    ofSetHexColor(0xffffff);
    
    // we could draw the whole contour finder
    //contourFinder.draw(360,540);
    
    ofColor c(255,255, 255);
    for(int i=0;i < contourFinder.nBlobs;i++){
        blobRect = contourFinder.blobs.at(i).boundingRect;
        blobRect.x +=700; blobRect.y +=10;
        c.setHsb(i*64,255,255);
        ofSetColor(c);
        ofDrawRectangle(blobRect);
    }
    
    //draw line
    ofSetColor(0,255,0);
    ofDrawLine(700,60,1030,60);
    ofDrawLine(700,130,1030,130);
    
    if(blobRect.y<60){
        jump();
    }
        else if(blobRect.y>130){
            duck();
        }
        else{
            checkSound();
            /*rect.x = 50;
            rect.y = 50;
             */
            movetoStart();
            
        }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key){
        case ' ':
            bLearnBakground = true;
            break;
        case '+':
            threshold ++;
            if (threshold > 255) threshold = 255;
            break;
        case '-':
            threshold --;
            if (threshold < 0) threshold = 0;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

void ofApp::audioIn(ofSoundBuffer & input){
    
    curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (size_t i = 0; i < input.getNumFrames()-1; i++){
        left[i]        = input[i*2]*0.5;
        right[i]    = input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
}

void ofApp::jump(){
    checkSound();
    /*rect.x = 50;
    rect.y = 0;
    ofDrawRectangle(rect);
    */
    if(rect.y>=endJump){
        rect.y -= 5.0;
    }
    ofDrawRectangle(rect);
    
}

void ofApp::duck(){
    checkSound();
    /*rect.x = 50;
    rect.y = 100;
    ofDrawRectangle(rect);
     */
    if(rect.y<=endDuck){
         rect.y += 5.0;
    }
    ofDrawRectangle(rect);
    
}

void ofApp::movetoStart(){
    checkSound();
    /*rect.x = 50;
     rect.y = 100;
     ofDrawRectangle(rect);
     */
    if(rect.y!=startingY){
        if(rect.y<=startingY){
         rect.y += 5.0;
    }
        else{
             rect.y -= 5.0;
        }
    }
    ofDrawRectangle(rect);
    
}

void ofApp::checkSound(){
    if(curVol>=S_TRIGGER){
        ofSetColor(255,165,0);
    }
    ofDrawRectangle(rect);
    
    if(curVol>=0.05){
        ofSetColor(255,0,0);
        ofDrawRectangle(rect);
    }
}


