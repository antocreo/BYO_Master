#include "GraphSender.h"



void GraphSender::loadSettings(ofxMyo::Myo myMyo) {
    
    

    //bools for debug
    bPaused = false;
    bDrawMesh = false;
    bDrawBar = false;
    bDrawSine = false;

    
    //myMyo setup
    myMyo.setup();
    

}
//------------------------------------------- EXIT -----------------------------------//


void GraphSender::exit(ofxMyo::Myo myMyo) {
    myMyo.waitForThread();
    
}

//------------------------------------------- SETUP -----------------------------------//

void GraphSender::setup(ofxMyo::Myo myMyo) {

    myMyo.setup();
    ofSetVerticalSync(true);
    
    featVal = new float[arraySize];
    
    buffersize = 512;
    
    buffer01 = new float[buffersize];
    buffer02 = new float[buffersize];
    buffer03 = new float[buffersize];
    buffer04 = new float[buffersize];
    buffer05 = new float[buffersize];
    buffer06 = new float[buffersize];
    buffer07 = new float[buffersize];
    buffer08 = new float[buffersize];
    bufferAccelX = new float[buffersize];
    bufferAccelY = new float[buffersize];
    bufferAccelZ = new float[buffersize];

    
    
    myIndex = new int[arraySize];
    
    //all indices start from 0
    for (int i = 0; i<arraySize; i++) {
        myIndex [i] = NULL;
        
    }

    
    movieOrigin = 0;
    
}

//------------------------------------------- UPDATE -----------------------------------//


void GraphSender::update(ofxMyo::Myo myMyo) {
    

    
    //creating array with float value of each feature
    
    putValInArrayIndex(featVal[0], buffer01 , buffersize, 0);
    putValInArrayIndex(featVal[1], buffer02 , buffersize, 1);
    putValInArrayIndex(featVal[2], buffer03 , buffersize, 2);
    putValInArrayIndex(featVal[3], buffer04 , buffersize, 3);
    putValInArrayIndex(featVal[4], buffer05 , buffersize, 4);
    putValInArrayIndex(featVal[5], buffer06 , buffersize, 5);
    putValInArrayIndex(featVal[6], buffer07 , buffersize, 6);
    putValInArrayIndex(featVal[7], buffer08 , buffersize, 7);
    
    putValInArrayIndex(featVal[7], bufferAccelX , buffersize, 7);
    putValInArrayIndex(featVal[7], bufferAccelX , buffersize, 7);

    putValInArrayIndex(featVal[7], bufferAccelX , buffersize, 7);




    
}

//------------------------------------------- DRAW -----------------------------------//


void GraphSender::draw(ofxMyo::Myo myMyo) {
    
    
    ///// drawing data bar chart /////
    
    if(bDrawBar) {
        ofPushStyle();
        ofPushView();
        ofFill();
        drawBar(ofVec2f ( 10 , 40 ), featVal); //if you use kinect you need to do +movieOrigin!!!
        ofPopView();
        ofPopStyle();
    }
    
    if (bDrawSine) {
        
        float amplitude = 5;
        float xpos = 20;
        float ypos = 500;
        float wavelenght = 2;
        drawSine(buffer08, buffersize, xpos, 500, wavelenght, amplitude, ofColor (200,200,100), 1, myMyo);
       

      

        
    }
    
    
    else {
        ofDrawBitmapString("searching for face...", 10, 20);
    }
    
    if(bPaused) {
        ofSetColor(255, 0, 0);
        ofDrawBitmapString( "paused" , 10, 32);
    }
    
    
}




//------------------------------------------- KEY PRESSED -----------------------------------//



void GraphSender::keyPressed(int key, ofxMyo::Myo myMyo) {
    switch(key) {
        case 'r':
//            myMyo.reset();
            break;
        case 'm':
            bDrawMesh = !bDrawMesh;
            break;

        case 'b':
            bDrawBar = !bDrawBar;
            break;
       
        case 'p':
            bDrawPoints = !bDrawPoints;
            
            break;
            
        case 'g':
            bDrawSine = !bDrawSine;
            
            break;
            


            
    }
}





//------------------------------------- DRAW BAR INFO TOTAL -----------------------------------

void GraphSender::drawBar(ofVec2f position){
    
    ofPushMatrix();
    ofPushStyle();
    
    float scale = 5;
    float barsize = 10;
    float gap = 20;
    
    for(int i = 0; i < arraySize; i++) {
        ofSetColor(255,200,200);
        ofRect(position.x,position.y+i*(barsize+gap), featVal[i]*scale, barsize);
        ofDrawBitmapString(featName[i] + " " + ofToString(featVal[i]), position.x,position.y+i*(barsize+gap)+20);
//        cout << i << featName[i] << " " << featVal[i] << endl;
        
    }
    ofPopStyle();
    ofPopMatrix();
    
}

void GraphSender::drawBar(ofVec2f position , float *featData){
    
    ofPushMatrix();
    ofPushStyle();
    
    float scale = 5;
    float barsize = 10;
    float gap = 20;
    
    for(int i = 0; i < arraySize; i++) {
        ofSetColor(255,200,200);
        ofRect(position.x,position.y+i*(barsize+gap), featData[i]*scale, barsize);
        ofDrawBitmapString(featName[i] + " " + ofToString(featData[i]), position.x,position.y+i*(barsize+gap)+20);
//        cout << i << featName[i] << " " << featData[i] << endl;
        
    }
    ofPopStyle();
    ofPopMatrix();
    
}

//------------------------------------- DRAW sine chart -----------------------------------


void GraphSender::drawSine(float *dataPointer, int arraylength, float xpos, float ypos,
                        float maxWl, float maxAmp, ofColor color, float linewidth, ofxMyo::Myo myMyo){
    ofPushMatrix();
    ofTranslate(0, ypos);
    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    float amplitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), 1, maxAmp);
    float wl = ofMap(ofGetMouseX(), 0, ofGetWidth(), 1, maxWl);
    
    if (myMyo.getDevices().size()>0) {
        for (int x=0; x<arraylength; x++) {
            mesh.addVertex(ofVec2f(xpos + x*wl, dataPointer[x]*amplitude ));
            mesh.addColor(color);
        }
    }
    mesh.draw();
    ofPopStyle();
    ofPopMatrix();
    
    
    
}

//------------------------------------- put val in array -----------------------------------


void GraphSender::putValInArrayIndex(float value, float *destinationArray, int arraylength, int indexVal){
    
    
    destinationArray[myIndex[indexVal]] = value;
    myIndex[indexVal]++;
    
    if (myIndex[indexVal] == arraylength) {
        myIndex[indexVal]= 0;
    }
    
    
}






