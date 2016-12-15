#pragma once

#include "ofMain.h"
#include "ofxMyo.h"


class GraphSender {
public:
    
    void loadSettings(ofxMyo::Myo myMyo);
    
    void setup(ofxMyo::Myo myMyo);
    void update(ofxMyo::Myo myMyo);
    void draw(ofxMyo::Myo myMyo);
    void keyPressed(int key, ofxMyo::Myo myMyo);
    void exit(ofxMyo::Myo myMyo);

    void drawBar(ofVec2f position);
    void drawBar(ofVec2f position, float *featData);
    void drawSine(float *dataPointer, int arraylength, float xpos, float ypos,
                  float maxWl, float maxAmp, ofColor color, float linewidth, ofxMyo::Myo myMyo);
    void putValInArrayIndex(float value, float *destinationArray, int arraylength, int indexVal);
    
    
    int camWidth, camHeight;
    int movieWidth, movieHeight;
    int sourceWidth, sourceHeight;
    float movieOrigin;
    
//    ofxMyo::Myo myo;
    
    
    
    float *featVal;
    float *featValVideo;
    string featName[8] = {
        
        "mouth width",
        "mouth height",
        "left eyebrow height",
        "right eyebrow height",
        "left eye openess",
        "right eye openess",
        "jaw openness",
        "nostril flare"
        
    };
    
    
    ofDirectory dir;
    vector<ofVideoPlayer> playlist;
    int currentmovie;
    
    
    
    const int arraySize = 8;
    
    vector<float> tempVal[8];
    vector<float> tempValVideo[8];
    
    
    int buffersize;
    float   *buffer01,
            *buffer02,
            *buffer03,
            *buffer04,
            *buffer05,
            *buffer06,
            *buffer07,
            *buffer08,
            *bufferAccelX,
            *bufferAccelY,
            *bufferAccelZ;
    
    int *myIndex;
    
    
    //booleans
    bool bDrawMesh,bDrawBar , bDrawPoints ,bDrawSine;
    bool bUseCamera, bPaused;
    
    
    
    
    
};
