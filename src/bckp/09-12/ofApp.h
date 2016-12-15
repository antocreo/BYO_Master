#pragma once

#include "ofMain.h"
#include "ofxMyo.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "WriteFile.h"

#define HOST "localhost"
#define PORT 12345

#define NUM_PTS 800
#define NUM_SENS 8
#define ACCEL 3
#define ARRAYSIZE = 8


enum modality{ mode1,mode2,mode3,mode4, modeOSC };

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void saveXml();
    void putValInArrayIndex(float value, float *destinationArray, int arraylength, int indexVal);
    void drawSine(float *dataPointer, int arraylength, float xpos, float ypos,
                  float maxWl, float maxAmp, ofColor color);
    void drawBitmapVals(float xpos, float ypos);
    void drawChoir2(float *dataPointer, int arraylength, float xpos, float ypos, float zpos,float yMultiplier,
                          float radius, float sides, ofColor fillColor, ofPrimitiveMode mode);
    void drawChoir(float *dataPointer, int arraylength, float xpos, float ypos,float zpos,
                   float radius, float sides, ofColor fillColor, ofPrimitiveMode mode);

    
    void audioOut(ofSoundBuffer &outBuffer);
    float getRMS (float *myBuffer, int myBufferSize);
    
    ofxMyo::Myo myo;
    
    
        
    vector<float> sensor [NUM_SENS];
    
    
    ofMesh EMGMesh;
   
    
    //pointers for collecting the stream
    int *myIndex;
    float *sensorVal;
    float *accVal;

    //pointers for storing the data as a buffer
    int buffersize;
    float   *buffer[NUM_SENS];
    float   *bufferAcc[ACCEL];

    //this are simple arrays that we use for the OSC
    float bufSens[NUM_SENS];
    float bufAcc[ACCEL];

    
    float theta, angle, halfAngle;

    ofSoundBuffer sBuffer[NUM_SENS];
    
    double wavePhase;
    double pulsePhase;
    double sampleRate;

    
    mutex audioMutex;
    ofSoundBuffer lastBuffer;
    ofPolyline waveform;
    float *rms;
    
    ofVec3f *centroid[NUM_SENS];
    
    ofEasyCam cam;
    
    vector < string > linesOfTheFile;

    
    bool bRecord;
    WriteFile sensorsFile;
    WriteFile accelFile;

    
    ofxOscSender sender;
    ofxOscMessage oscAcc[ACCEL];
    ofxOscMessage oscSens[NUM_SENS];

    modality modality;
    
    
    
};
