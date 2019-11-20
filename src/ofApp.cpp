#include "ofApp.h"
#include <stdlib.h>


//--------------------------------------------------------------
void ofApp::setup(){
    
    //Sound
    ofSetVerticalSync(true);
    soundStream.printDeviceList();
    int bufferSize = 256;
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    smoothedVol = 0.0;
    scaledVol = 0.0;
    ofSoundStreamSettings settings;
    auto devices = soundStream.getMatchingDevices("default");
    if (!devices.empty()) {
        settings.setInDevice(devices[0]);
    }
    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    //Video
    YPixel=0;
    vidGrabber.setVerbose(true);
    vidGrabber.setup(320, 240);
    colorImg.allocate(320, 240);
    grayImage.allocate(320, 240);
    grayBg.allocate(320, 240);
    grayDiff.allocate(320, 240);
    bLearnBakground = true;
    threshold = 60;
    
    //Background
    background.load("background.jpg");
    
    
    //Animation
    startingX = 30;
    startingY = 200;
    endJump = 80;
    endDuck = 300;
    charX = startingX;
    charY = startingY;
    character.load("Monster.png");
    character.draw(charX,charY,100,100);
    blobRect.x = startingX;
    blobRect.y = startingY;
    
    //meteor
    meteor.load("meteor.png");
    
    //enemy
    enemy.load("spaceship.png");
    
    //life
    heart1.load("heart.png");
    heart2.load("heart.png");
    heart3.load("heart.png");
    heart3.draw(590,15);
    heart2.draw(530,15);
    heart1.draw(470,15);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(gamestate==true){
    //Audio
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    //Video
    ofBackground(100, 100, 100);
    bool bNewFrame = false;
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    if (bNewFrame) {
        colorImg.setFromPixels(vidGrabber.getPixels());
        grayImage = colorImg;
        if (bLearnBakground == true) {
            grayBg = grayImage;        // the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
        grayDiff.erode_3x3();
        grayDiff.dilate_3x3();
        contourFinder.findContours(grayDiff, 2000, 30000, 1 ,false, true);
    }
    
    //meteor
    meteor.update();
    meteorX-=3;
    
    //character
    character.update();
    
    //enemy
    enemy.update();
    enemyX -= 3;
    
    //life
    heart1.update();
    heart2.update();
    heart3.update();
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(gamestate==true){
    //background
    ofSetColor(255, 255, 255);
    background.draw(10, 10, backgroundWidth, backgroundHeight);
    
    //Video
    // draw the incoming, the grayscale, the bg and the thresholded difference
    ofSetHexColor(0xffffff);
    grayDiff.draw(grayDiffX, grayDiffY);
    
    ofColor c(255, 255, 255);
    
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        blobRect = contourFinder.blobs.at(i).boundingRect;
        blobRect.x += 660; blobRect.y += 130;
        c.setHsb(i * 64, 255, 255);
        ofSetColor(c);
        ofDrawRectangle(blobRect);
    }
    
    /*
    ofPixels & pixels = grayDiff.getPixels();
    YPixel=999;
    for(int i = 0; i < grayDiff.width; i++){
        for(int j=0; j < grayDiff.height; j++){
            unsigned char grayValue= pixels[j * grayDiff.width + i];
        if ((grayValue>0) && (j<YPixel)) {
            YPixel=j+grayDiffY;
            //break;
         }
        }
    }
    //cout<<YPixel<<endl;
    */
    //draw line (Trigger)
    ofSetColor(0, 255, 0);
    ofDrawLine(grayDiffX, jumpTrigger, (grayDiffX + 320), jumpTrigger);
    ofDrawLine(grayDiffX, duckTrigger, (grayDiffX + 320), duckTrigger);
    //blobRect.y or YPixel
    if ( blobRect.y<jumpTrigger) {
        jump();
    }
    
    //blobRect.y or YPixel
    else if (blobRect.y>duckTrigger) {
        duck();
    }
    
    else {
        movetoStart();
    }
    
    //meteor
    ofSetColor(255, 255, 255);
    if (meteorX <= 10) {
        meteorX = 570;
        meteorY = generateRandomY();
    }
    meteor.draw(meteorX, meteorY, 100, 100);
    
  
    //enemy
    checkSound();
    enemy.draw(enemyX, enemyY);
    if (enemyX <= 10) {
        enemyX = 570;
    }
    
    //Parameters
    //ofDrawBitmapString(ofToString(curVol,2), grayDiffX, 400);
    ofDrawBitmapString(ofToString(YPixel,2), grayDiffX, 420);
    ofDrawBitmapString(ofToString(lifeCounter,2), grayDiffX, 440);
    ofDrawBitmapString(ofToString(meteorX,2), grayDiffX, 460);
    
    //life
    checkForCollisions();
    switch (lifeCounter){
        case 1:
            heart1.draw(470,15);
            heart2.clear();
            heart3.clear();
            break;
            
        case 2:
            heart2.draw(530,15);
            heart1.draw(470,15);
            heart3.clear();
            break;
        
        case 3:
            heart3.draw(590,15);
            heart2.draw(530,15);
            heart1.draw(470,15);
            break;
            
    }
    }
    
    else{
        background.draw(10,10,backgroundWidth,backgroundHeight);
        background.load("start.jpg");
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case ' ':
            bLearnBakground = true;
            break;
        case '+':
            threshold++;
            if (threshold > 255) threshold = 255;
            break;
        case '-':
            threshold--;
            if (threshold < 0) threshold = 0;
            break;
        case 's':
            gamestate=true;
            background.load("background.jpg");
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

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input) {
    
    curVol = 0.0;
    // samples are "interleaved"
    int numCounted = 0;
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (size_t i = 0; i < input.getNumFrames()-1; i++) {
        left[i] = input[i * 2] * 0.5;
        right[i] = input[i * 2 + 1] * 0.5;
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted += 2;
    }
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    // this is how we get the root of rms :)
    curVol = sqrt(curVol);
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    
}

//--------------------------------------------------------------
void ofApp::jump() {

    if (charY >= endJump) {
        charY -= 5.0;
    }
    ofSetColor(255, 255, 255);
    character.draw(charX,charY,100,100);
}

//--------------------------------------------------------------
void ofApp::duck() {
   
    if (charY <= endDuck) {
        charY += 5.0;
    }
    ofSetColor(255, 255, 255);
    character.draw(charX, charY,100,100);
}

//------------------------------
void ofApp::checkSound() {
    if (curVol >= S_TRIGGER) {
        scare();
    }
    
}

//------------------------------
void ofApp::movetoStart() {
    
    if (charY != startingY) {
        if (charY <= startingY) {
            charY += 5.0;
        }
        else {
            charY -= 5.0;
        }
    }
    ofSetColor(255, 255, 255);
    character.draw(charX, charY, 100, 100);
    
}

void ofApp::scare() {
    
    enemyY -= 3;
    if (enemyY <= 0) {
        enemyX = 570;
        enemyY = generateRandomY();
    }

}

int ofApp::generateRandomY(){
    int newY= (rand()%(backgroundHeight-100))+10;
    return (newY);
}

void ofApp::checkForCollisions(){
 int midpointXE=enemyX+50;
 int midpointYE=enemyY+50;
 int midpointXM=meteorX+50;
 int midpointYM=meteorY+50;
 if(midpointXE>=charX&&midpointXE<=charX+100&&midpointYE>=charY&&midpointYE<=charY+100){
        if(lifeCounter>0){
            lifeCounter--;
            enemyX = 570;
            enemyY = generateRandomY();
        }
    }
    else if(midpointXM>=charX&&midpointXM<=charX+100&&midpointYM>=charY&&midpointYM<=charY+100){
            if(lifeCounter>0){
                lifeCounter--;
                meteorX = 570;
                meteorY = generateRandomY();
            }
    }
}

//------------------------------

