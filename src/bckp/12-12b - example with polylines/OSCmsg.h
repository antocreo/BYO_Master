#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 6448


#define NUM_SENS 8
#define ACCEL 3
#define ARRAYSIZE = 8

class OSCmsg {
public:
    void setup(string name, int loopSize);
    void update(float data, int loopSize, bool xml, bool txt, int samplingTime, string name);
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void gotMessage(ofMessage msg);
    
    vector<float> sensor [NUM_SENS];
    
    int folder;
    string path;
    
    int *myIndex;
    float *sensorVal;
    
    int buffersize;
    float   *buffer[NUM_SENS];
    
    ofBuffer bufferAcc[ACCEL], bufferEMG[NUM_SENS];
 
    
    ofxOscSender sender;

    
    
};
