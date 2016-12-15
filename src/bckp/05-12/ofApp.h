#pragma once

#include "ofMain.h"
#include "ofxMyo.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 12345

#define NUM_PTS 800
#define NUM_SENS 8
#define ACCEL 3
#define ARRAYSIZE = 8

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
                  float maxWl, float maxAmp, ofColor color, float linewidth);
    void drawBitmapVals(float xpos, float ypos);
    void drawChoir(float *dataPointer, int arraylength, float xpos, float ypos,
                          float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode);
    void drawChoir(float *dataPointer, int arraylength, float xpos, float ypos,float zpos,
                   float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode);

    
    void audioOut(ofSoundBuffer &outBuffer);
    float getRMS (float *myBuffer, int myBufferSize);
    
    ofxMyo::Myo myo;
    
    ofxOscSender sender;
    ofxOscMessage oscMsg;
    
    ofxXmlSettings XML[NUM_SENS] ;
    ofxXmlSettings XMLAcc[ACCEL];
    ofTrueTypeFont TTF;
    
    vector <string> xmlStructure;
    string message;
    
    ofPoint dragPts[NUM_PTS];
    
    int pointCount;
    int lineCount;
    vector <int> lastTagNumber;
    
    float red;
    float green;
    float blue;
    
    vector<float> sensor [NUM_SENS];
    
    int folder;
    string path;
    
    ofMesh EMGMesh;
//    ofVboMesh mesh;
   
    int *myIndex;
    float *sensorVal;
    
    int buffersize;
    float   *buffer[NUM_SENS];
    
    float theta, angle, halfAngle;

    ofSoundBuffer sBuffer[NUM_SENS];
    
    double wavePhase;
    double pulsePhase;
    double sampleRate;

    
    mutex audioMutex;
    ofSoundBuffer lastBuffer;
    ofPolyline waveform;
    float *rms;
    
    ofVec3f centroid;
    
    ofEasyCam cam;
    
    int modality;
    
};
