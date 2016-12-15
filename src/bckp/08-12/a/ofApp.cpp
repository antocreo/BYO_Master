#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //some general setup
    ofSetVerticalSync(true);
    cam.setDistance(100);
    ofSetCircleResolution(64);
    ofBackground(0);
    
    //--------setup myo
    myo.setup();
    
    //-------- file recording setup
    bRecord = false;
    sensorsFile.setup("sensor", NUM_SENS);
    
    
    //define the buffer. I set 512 to be used for some future app with the audio but can be anything atm
    buffersize = 100;
    
    //initialise the buffer array of pointers with array of floats of size buffersize (it's an array of array basically)
    //this will store the stream of values ccoming from each sensor
    for (int j = 0; j<NUM_SENS; j++) {
        buffer[j] = new float[buffersize];      //main buffer
        centroid[j] = 0;
    }
    
    myIndex = new int[NUM_SENS];            //for circular array
    sensorVal = new float[NUM_SENS];        //init. feat value array of pointers these will store the sensor values and pass them to the buffer
    rms = new float [NUM_SENS];             //init rms for each EMG
    
    
    //all indices of the circular buffers start from NULL
    for (int i = 0; i<NUM_SENS; i++) {
        myIndex [i] = NULL;
    }
    
    sampleRate = 44100;
    wavePhase = 0;
    pulsePhase = 0;
    
    modality = 1;
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
//        buffersize = ofClamp(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 512),0,512);
    
    
    for ( int i=0; i<myo.getDevices().size(); i++ ) {
        
        //get and save Accel data
        for (int a = 0; a<ACCEL; a++) {
            
            //            XMLAcc[a].addValue("Acc_" + ofToString(a), myo.getDevices()[i]->getAccel().getPtr()[a]);
            //XMLAcc[a].saveFile(path + "/AccData_" + ofToString(a) + ".xml");
            
            //save the file
            //            sensorsFile.update(myo.getDevices()[i]->getAccel().getPtr()[a], ACCEL, true, true, 60, "Accel");
            
        }
        
        //get and save 8 sensor data
        for (int j = 0; j<NUM_SENS; j++) {
            
            //---------------let's fill the array with the sensor data
            sensorVal[j] = myo.getDevices()[i]->getEmgSamples()[j];
            putValInArrayIndex(sensorVal[j], buffer[j], buffersize, j);
            
            //---------------fill the audioBuffer
            //sBuffer[j].copyFrom(buffer[j], buffersize, 1, sampleRate);
            //get the rms for each EMG sensor
            //rms[j] = sBuffer[j].getRMSAmplitude();
            
            //---------------get the rms of the buffer
            rms[j] = getRMS(buffer[j], buffersize);
            
            
            //---------------Recording File
            //let's read each sensor value and store it in the xml file
            //            XML[j].addValue("S_" + ofToString(j), myo.getDevices()[i]->getEmgSamples()[j]);
            //XML[j].saveFile(path + "/sensorData_" + ofToString(j) + ".xml");
            
            //save file if pressed 'r'
            if (bRecord) {
                sensorsFile.update(sensorVal[j], NUM_SENS, true, true, 60, "Sens");
            }
            
            
        }
    }
    
    //---------- cam setting------------//
    
    //    cam.setDistance(500+myo.getDevices()[0]->getGyro().x);
    cam.setDistance(500);
    
    
  
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if (myo.getDevices().size()>0) {
        
        
        //drawbitmap the values
        drawBitmapVals(10, 10);
        ofDrawBitmapString("RMS " + ofToString(getRMS(buffer[0],buffersize)), 10, 400);
        ofDrawBitmapString("Roll " + ofToString(ofRadToDeg(myo.getDevices()[0]->getRoll())), 10, 420);
        ofDrawBitmapString("Yaw " + ofToString(ofRadToDeg(myo.getDevices()[0]->getYaw())), 10, 440);
        ofDrawBitmapString("Pitch " + ofToString(ofRadToDeg(-myo.getDevices()[0]->getPitch())), 10, 460);
        ofDrawBitmapString("Modality " + ofToString(modality), 10, 480);
        
        
        // bar chart of the sensors
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            for ( int j=0; j<NUM_SENS; j++ ) {
                //draw some bars
                ofDrawRectangle(10 + (j * (10+2)) , ofGetHeight() - 100, 10, myo.getDevices()[i]->getEmgSamples()[j]);
            }
        }
        
        
        //start camera
        cam.begin();
        
        
        //mesh setting
        ofPushMatrix();
        
        
        for ( int j=0; j<NUM_SENS; j++ ) {
           
            
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            
            //float *dataPointer, int arraylength, float xpos, float ypos, float zpos, float yMultiplier, float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode
            //let's try drawing some cool circle
            
            //----modality 1 point cloud
            if (modality == 1) {
                float k = 2;
                drawChoir2(buffer[j], buffersize,  0, 0, 0, 2*rms[j], 10*rms[j], buffersize, ofColor(255/(j+1),100+rms[j], 255-j, 200), OF_PRIMITIVE_POINTS);
                drawChoir2(buffer[j], buffersize,  0, 0, 0, 2*rms[j], 10*rms[j], buffersize, ofColor(255/(j+1),100+rms[j], 255-j, 10*rms[j]), OF_PRIMITIVE_LINE_LOOP);
            }
            
            //----modality 2 circular polygon
            if (modality == 2){

                drawChoir(buffer[j], buffersize, 0, 0, 0, rms[j] * 10, buffersize, ofColor(255/(rms[j]+1),250, 250, 200), OF_PRIMITIVE_LINE_LOOP);
                drawChoir(buffer[j], buffersize, 0, 0, 0, rms[j] * 10, buffersize , ofColor(255/(rms[j]+1),250, 250, 40), OF_PRIMITIVE_TRIANGLE_STRIP);
                
            }
            
        }
        
        ofPopMatrix();
        
        cam.end();
        
        for ( int j=0; j<NUM_SENS; j++ ) {
            //----modality 3 simple sine graph

            if (modality == 3) {
                float xpos = 100;
                float ypos = 200;
                float gap = 40;
                float maxWl = 3;
                float maxAmp = 1.2;
                //float *dataPointer, int arraylength, float xpos, float ypos,float maxWl, float maxAmp, ofColor color
                drawSine(buffer[j], buffersize, xpos, ypos+(gap*j), maxWl, maxAmp, ofColor (10*(rms[j]),100,20));
            }
            
        }
        
    }
    
    
    
}

//------------------------------------- EXIT -----------------------------------


void ofApp::exit(){
    myo.stop();
}



//------------------------------------- DRAW sine chart -----------------------------------


void ofApp::drawSine(float *dataPointer, int arraylength, float xpos, float ypos,
                     float maxWl, float maxAmp, ofColor color){
    
    
    ofPushMatrix();
    float cursor = ofGetMouseX();
    
    ofTranslate(xpos, ypos);
    
    
    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    float amplitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), 1, maxAmp);
    //    float wl = ofMap(ofGetMouseX(), 0, ofGetWindowWidth(), 0, maxWl);
    float wl =  maxWl;
    
    for (int x=0; x<arraylength; x++) {
        cursor=x+wl;
        mesh.addVertex(ofVec2f(xpos + x*wl, dataPointer[x]*amplitude ));
        mesh.addColor(color);
        
    }
    
    ofVec3f thisCentroid = mesh.getCentroid();
    mesh.draw();
    
    
    ofPopStyle();
    ofPopMatrix();
    ofDrawBitmapString(ofToString(wl), 200, 200);
    
}



//------------------------------------- DRAW Choir2 chart -----------------------------------


void ofApp::drawChoir2(float *dataPointer, int arraylength, float xpos, float ypos,float zpos, float yMultiplier,
                       float radius, float sides, ofColor fillColor, ofPrimitiveMode mode){
    
    ofPushMatrix();
    ofTranslate(xpos,ypos,zpos);
    
    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(mode);
    
    angle = TWO_PI / sides;
    theta = 0;
    halfAngle = angle/2.0+sin(theta);
    
    for (int x=0; x<arraylength; x++) {
        
        float xp = xpos  + dataPointer[x] + cos(x+angle) * radius;
        float yp = ypos  + dataPointer[x] + sin(x+angle) * radius;
        float zp = x * sqrt((pow(myo.getDevices()[0]->getAccel().x,2) +
                             pow (myo.getDevices()[0]->getAccel().y,2) +
                             pow(myo.getDevices()[0]->getAccel().z,2))/3);
        
        
        mesh.addVertex(ofVec3f(xp, yp, zpos + (yMultiplier)));
        
        mesh.addColor(fillColor);
        
        
        if (x < arraylength-1) {
            mesh.addIndex(x);
        }else{
            
            mesh.addIndex(0);
        }
        
        //display vertex index
        //ofDrawBitmapString(ofToString(mesh.getIndex(x)), xp, yp);
        
        
    }
    
    //control rotation with Roll
    ofRotateZ(ofRadToDeg(myo.getDevices()[0]->getRoll()));
    ofRotateY(ofRadToDeg(myo.getDevices()[0]->getYaw()));
    ofRotateX(ofRadToDeg(myo.getDevices()[0]->getPitch()));
    ofVec3f thisCentroid = mesh.getCentroid();
    
    ofTranslate(thisCentroid);
    
    mesh.draw();
    
    
    ofPopStyle();
    ofPopMatrix();
    
}

//------------------------------------- DRAW Choir chart  -----------------------------------


void ofApp::drawChoir(float *dataPointer, int arraylength, float xpos, float ypos, float zpos,
                      float radius, float sides, ofColor fillColor, ofPrimitiveMode mode){
    
    ofPushMatrix();
    ofTranslate(xpos, ypos, zpos);
    
    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(mode);
    
    angle = TWO_PI / sides;
    theta = 0;
    halfAngle = angle/2.0+sin(theta);
    
    
    for (int x=0; x<arraylength; x++) {
        
        //creating a polygon with the num of side of the buffer size
        float xp = xpos + cos(x*angle) * (radius + dataPointer[x]);
        float yp = ypos + sin(x*angle) * (radius + dataPointer[x]);
        float zp = zpos;
        
        
        mesh.addVertex(ofVec3f(xp, yp, zp ));
        mesh.addColor(fillColor);
        
        if (x < arraylength-1) {
            mesh.addIndex(x);
        }else{
            
            mesh.addIndex(arraylength-1);
        }
        //display vertex index
        //        ofDrawBitmapString(ofToString(mesh.getIndex(x)), xp, yp);
        
    }
    
    /* //test with polylines
     ofFill();
     ofSetColor(fillColor);
     ofSetPolyMode(OF_POLY_WINDING_ODD);
     ofPolyline poly;
     ofPath path;
     for (int x=0; x<arraylength; x++) {
     
     float xp = xpos + cos(x*angle) * (radius + dataPointer[x]);
     float yp = ypos + sin(x*angle) * (radius + dataPointer[x]);
     float zp = zpos;
     
     poly.addVertex(xp, yp, zp);
     if(x == 0) {
     path.newSubPath();
     path.moveTo(poly.getVertices()[x] );
     } else {
     path.lineTo( poly.getVertices()[x] );
     }
     }
     
     poly.close();
     path.close();
     path.simplify();
     */
    
    
    
    //control rotation with Roll
    ofRotateZ(ofRadToDeg(myo.getDevices()[0]->getRoll()));
    ofRotateY(ofRadToDeg(myo.getDevices()[0]->getYaw()));
    ofRotateX(ofRadToDeg(-myo.getDevices()[0]->getPitch()));
    
    ofVec3f thisCentroid = mesh.getCentroid();
    
    ofTranslate(thisCentroid);
    mesh.draw();
    //    path.draw(xpos, ypos);
    //    poly.draw();
    
    
    ofPopStyle();
    ofPopMatrix();
    
}

//------------------------------------- put val in array -----------------------------------

//this is a circular buffer. it takes a value and it puts it into the destination Array

void ofApp::putValInArrayIndex(float value, float *destinationArray, int arraylength, int indexVal){
    
    
    destinationArray[myIndex[indexVal]] = value;
    myIndex[indexVal]++;
    
    if (myIndex[indexVal] == arraylength) {
        myIndex[indexVal]= 0;
    }
    
    
}
//------------------------------------- RMS  -----------------------------------


float ofApp::getRMS (float *myBuffer, int myBufferSize){
    
    
    double acc = 0;
    for(int i = 0; i < myBufferSize; i++){
        acc += myBuffer[i] * myBuffer[i];
    }
    return sqrt(acc / (double)myBufferSize);
    
}

//------------------------------------- Draw bitmap values -----------------------------------


void ofApp::drawBitmapVals(float xpos, float ypos){
    
    
    for ( int i=0; i<myo.getDevices().size(); i++ ) {
        stringstream s;
        s << "id: " << myo.getDevices()[i]->getId() << endl;
        s << "which: " << myo.getDevices()[i]->getWhichArm() << endl;
        s << "pose: " << myo.getDevices()[i]->getPose() << endl;
        s << "accel:          ";
        s << myo.getDevices()[i]->getAccel().x << ",";
        s << myo.getDevices()[i]->getAccel().y << ",";
        s << myo.getDevices()[i]->getAccel().z << endl;
        
        
        
        s << "gyro:           ";
        s << myo.getDevices()[i]->getGyro().x << ",";
        s << myo.getDevices()[i]->getGyro().y << ",";
        s << myo.getDevices()[i]->getGyro().z << endl;
        s << "quaternion:     ";
        s << myo.getDevices()[i]->getQuaternion().x() << ",";
        s << myo.getDevices()[i]->getQuaternion().y() << ",";
        s << myo.getDevices()[i]->getQuaternion().z() << ",";
        s << myo.getDevices()[i]->getQuaternion().w() << endl;
        s << "roll/pitch/yaw: ";
        s << myo.getDevices()[i]->getRoll() << ",";
        s << myo.getDevices()[i]->getPitch() << ",";
        s << myo.getDevices()[i]->getYaw() << endl;
        s << "raw data:       ";
        
        for ( int j=0; j<NUM_SENS; j++ ) {
            s << myo.getDevices()[i]->getEmgSamples()[j];
            s << ",";
            
        }
        s << endl;
        ofSetColor(255);
        ofDrawBitmapString(s.str(), xpos, ypos + i*100);
    }
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    sensorsFile.keyPressed(key);
    
    if (key== 's') {
        
        //        //create dir if doesn't exist
        //        path =ofToString(ofGetDay()) +"_"+ ofToString(ofGetHours())+"_"+ ofToString(ofGetMinutes())+"_"+ ofToString(ofGetSeconds());
        //        ofDirectory dir(path);
        //        if(!dir.exists()){
        //            dir.create(true);
        //        }
        bRecord = !bRecord;
        
        
    }
    
    modality = key - 48;
    
    
    
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
    
    
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
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    //
    //    // base frequency of the lowest sine wave in cycles per second (hertz)
    //    float frequency = 440;
    //
    //    // mapping frequencies from Hz into full oscillations of sin() (two pi)
    //    float wavePhaseStep = (frequency / sampleRate) * TWO_PI;
    //    float pulsePhaseStep = (0.5 / sampleRate) * TWO_PI;
    //
    //    // this loop builds a buffer of audio containing 3 sine waves at different
    //    // frequencies, and pulses the volume of each sine wave individually. In
    //    // other words, 3 oscillators and 3 LFOs.
    //
    //    for(size_t i = 0; i < outBuffer.getNumFrames(); i++) {
    //
    //        // build up a chord out of sine waves at 3 different frequencies
    //        float sampleLow = sin(wavePhase);
    //        float sampleMid = sin(wavePhase * 1.5);
    //        float sampleHi = sin(wavePhase * 2.0);
    //
    //        // pulse each sample's volume
    //        sampleLow *= sin(pulsePhase);
    //        sampleMid *= sin(pulsePhase * 1.04);
    //        sampleHi *= sin(pulsePhase * 1.09);
    //
    //        float fullSample = (sampleLow + sampleMid + sampleHi);
    //
    //        // reduce the full sample's volume so it doesn't exceed 1
    //        fullSample *= 0.3;
    //
    //        // write the computed sample to the left and right channels
    //        outBuffer.getSample(i, 0) = fullSample;
    //        outBuffer.getSample(i, 1) = fullSample;
    //        
    //        // get the two phase variables ready for the next sample
    //        wavePhase += wavePhaseStep;
    //        pulsePhase += pulsePhaseStep;
    //    }
    //    
    //    unique_lock<mutex> lock(audioMutex);
    //    lastBuffer = outBuffer;
}

//--------------------------------------------------------------

