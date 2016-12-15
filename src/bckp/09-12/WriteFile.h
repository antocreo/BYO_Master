#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"


#define NUM_SENS 8
#define ACCEL 3
#define ARRAYSIZE = 8

class WriteFile {
public:
    void setup(string name, int loopSize);
    void update(float data, int loopSize, bool xml, bool txt, int samplingTime, string name);
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
   
    
    ofxXmlSettings XML[NUM_SENS] ;
    ofxXmlSettings XMLAcc[ACCEL];
    
     
    vector<float> sensor [NUM_SENS];
    
    int folder;
    string path;
   
    int *myIndex;
    float *sensorVal;
    
    int buffersize;
    float   *buffer[NUM_SENS];
        
    ofBuffer bufferAcc[ACCEL], bufferEMG[NUM_SENS];
    
    ofFile fileAcc[ACCEL];
    ofFile fileEMG[NUM_SENS];
    
    
    
};
