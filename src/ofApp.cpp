#include "ofApp.h"
#include <stdlib.h>


//--------------------------------------------------------------
void ofApp::setup(){
    
    //Countdown
    ofBackground(255,255,255);
    ofSetVerticalSync(true);
    countdown.load("Countdown.mov");
    countdown.setLoopState(OF_LOOP_NONE);
    
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
    meteor2.load("meteor.png");

    
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
    if(gamestate==true && lifeCounter!=0){
    //Countdown
    countdown.update();
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
        //contourFinder.findContours(grayDiff, 2000, 30000, 1 ,false, true);
        ofPixels & pixels = grayDiff.getPixels();
        YPixel=999;
        for(int y = grayDiff.height;y>=0; y--){
            for(int x=0; x< grayDiff.width; x++){
                unsigned char grayValue= pixels[y * grayDiff.width + x];
                if ((grayValue>0) && (y<YPixel)) {
                    YPixel=y+grayDiffY;
                    break;
                }
            }
        }
    }
    
    
    //meteor
    meteor.update();
    meteorX-=2;
    meteor2.update();
    meteorX2-=4;
    
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
    if(gamestate==true&& lifeCounter!=0){
    //Countdown
    ofSetColor(255, 255, 255);
    countdown.draw(10,10, backgroundWidth, backgroundHeight);
    bool done=countdown.getIsMovieDone();
    if (done){
    //background
    ofSetColor(255, 255, 255);
    background.draw(10, 10, backgroundWidth, backgroundHeight);
    
    //Video
    // draw the incoming, the grayscale, the bg and the thresholded difference
    ofSetHexColor(0xffffff);
    grayDiff.draw(grayDiffX, grayDiffY);
    
    /*
    ofColor c(255, 255, 255);
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        blobRect = contourFinder.blobs.at(i).boundingRect;
        blobRect.x += 660; blobRect.y += 130;
        c.setHsb(i * 64, 255, 255);
        ofSetColor(c);
        ofDrawRectangle(blobRect);
    }
    */
    

    //draw line (Trigger)
    ofSetColor(0, 255, 0);
    ofDrawLine(grayDiffX, jumpTrigger, (grayDiffX + 320), jumpTrigger);
    ofDrawLine(grayDiffX, duckTrigger, (grayDiffX + 320), duckTrigger);
    ofDrawLine(grayDiffX, YPixel, (grayDiffX + 320), YPixel);
    //blobRect.y or YPixel
    if ( YPixel<jumpTrigger) {
        jump();
    }
    
    //blobRect.y or YPixel
    else if (YPixel>duckTrigger) {
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
    
    ofSetColor(255, 255, 255);
    if (meteorX2 <= 10) {
        meteorX2 = 570;
        meteorY2 = generateRandomY();
    }
    meteor2.draw(meteorX2, meteorY2, 100, 100);
    
  
    //enemy
    checkSound();
    enemy.draw(enemyX, enemyY);
    if (enemyX <= 10) {
        enemyX = 570;
    }
    
    //Parameters
    ofDrawBitmapString(ofToString(curVol,2), grayDiffX, 400);
    ofDrawBitmapString(ofToString(lifeCounter,2), grayDiffX, 440);
    
    
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
    }
    
    else if (lifeCounter ==0){
        background.load("gameover.jpg");
        background.draw(10,10,backgroundWidth,backgroundHeight);
    }
    else if (lifeCounter>0&&gamestate==false) {
        background.load("start.jpg");
        background.draw(10,10,backgroundWidth,backgroundHeight);
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
            countdown.play();
            break;
        case 'a':
            gamestate=false;
            lifeCounter=3;
            setup();
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
    
    enemyY -= 4;
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
 int midpointXM2=meteorX2+50;
 int midpointYM2=meteorY2+50;
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
    else if(midpointXM2>=charX&&midpointXM2<=charX+100&&midpointYM2>=charY&&midpointYM2<=charY+100){
        if(lifeCounter>0){
            lifeCounter--;
            meteorX2 = 570;
            meteorY2 = generateRandomY();
        }
    }
}

//------------------------------

