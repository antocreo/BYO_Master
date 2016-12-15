#pragma once

#include "ofMain.h"
#include "ofxMyo.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "WriteFile.h"
#include "ofxMaxim.h"
#include "ofxBullet.h"
#include "GUI.h"
#include "Pulse.h"

#define HOST "localhost"
#define PORT 12345

#define NUM_PTS 800
#define NUM_SENS 8
#define ACCEL 3
#define MYBUFSIZE  20


//enum modality{ mode1,mode2,mode3,mode4 };

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
    void putValInArrayIndex(float value, float *destinationArray, int arraylength, int indexVal, int* myPointer);
    void drawSine(float *dataPointer, int arraylength, float xpos, float ypos,
                  float maxWl, float maxAmp, ofColor color);
    void drawBitmapVals(float xpos, float ypos);
    void drawChoir2(float *dataPointer, int arraylength, float xpos, float ypos, float zpos,float yMultiplier,
                          float radius, float sides, ofColor fillColor, ofPrimitiveMode mode);
    void drawChoir(float *dataPointer, int arraylength, float xpos, float ypos,float zpos, 
                   float radius, float sides, ofColor fillColor, ofPrimitiveMode mode);

    
    float getRMS (float *myBuffer, int myBufferSize);
    float lopass(float *input, float cutoff);
    
    
///--- ofxBullet
    void onCollision(ofxBulletCollisionData& cdata);
    void mousePickEvent( ofxBulletMousePickEvent &e );

    ofxBulletWorldRigid			world;
    vector <ofxBulletBox*>		bounds;
    float						boundsWidth;
    btBoxShape*					boxShape;
    btSphereShape*				sphereShape;
    vector<ofxBulletRigidBody*>	shapes;
    vector<bool>				bColliding;
    int							mousePickIndex;
    ofVec3f						mousePickPos;
    
///---- end ofxBullet
    
    
///--- audio

    void audioOut(ofSoundBuffer &outBuffer);
    
//    double wavePhase;
//    double pulsePhase;
//    double sampleRate;
//    ofSoundBuffer sBuffer;
//    
//    mutex audioMutex;
//    ofSoundBuffer lastBuffer;
//    ofPolyline waveform;
//    float rms;
//    
//    
//    float           nextLetterTime;
//    int             lineCount;
//    int             letterCount;
//    vector <string> seussLines;
//    float frequency;
//    int temp;


    Pulse myPulse[NUM_SENS];
    Pulse myPulse1;
    Pulse myPulse2;

///--- end of audio

    
    ofxMyo::Myo myo;
    
    vector<float> sensor [NUM_SENS];
    
    ofMesh EMGMesh;
   
    //pointers for collecting the stream
    int *myIndex1;
    int *myIndex2;
    int *myIndex3;
    int *myIndexSensOSC;
    int *myIndexAccOSC;

    float *sensorVal;
    float *accVal;

    //pointers for storing the data as a buffer
    int buffersize;
    float   *buffer[NUM_SENS];
    float   *bufferAcc[ACCEL];

    //this are simple arrays that we use for the OSC
    float bufSens[NUM_SENS];
    float bufAcc[ACCEL];

    float lastAcc, lastSens;
    
    float theta, angle, halfAngle;

    float *rmsSens;
    float *rmsAcc;
    
    float avrgRMSSens, avrgRMSAcc;

    
    ofVec3f *centroid[NUM_SENS];
    
    ofCamera cam;
    
    vector < string > linesOfTheFile;
    
    WriteFile sensorsFile[NUM_SENS];
    WriteFile accelFile[ACCEL];

    ofxOscSender sender;
    ofxOscMessage oscAcc[ACCEL];
    ofxOscMessage oscSens[NUM_SENS];

    GUI gui;
    
    
    
    
};
