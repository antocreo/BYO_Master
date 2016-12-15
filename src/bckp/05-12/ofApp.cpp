#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    

    
    //some general setup
    ofSetVerticalSync(true);
//    ofEnableDepthTest();
    cam.setDistance(100);
    ofSetCircleResolution(64);
    ofBackground(0);


    //setup myo
    myo.setup();
    
    //create dir if doesn't exist
    path =ofToString(ofGetDay()) +"_"+ ofToString(ofGetHours())+"_"+ ofToString(ofGetMinutes())+"_"+ ofToString(ofGetSeconds());
    ofDirectory dir(path);
    if(!dir.exists()){
        dir.create(true);
    }
    
    //clear the xml, just in case
    for (int j = 0; j<NUM_SENS; j++) {
        
        XML[j].clear();
    }
    
    //define the buffer. I set 512 to be used for some future app with the audio but can be anything atm
    buffersize = 20;
    
    //initialise the buffer array of pointers with array of floats of size buffersize (it's an array of array basically)
    //this will store the stream of values ccoming from each sensor
    for (int j = 0; j<NUM_SENS; j++) {
        buffer[j] = new float[buffersize];
        sBuffer[j].allocate(buffersize, 1);
    }
    
    myIndex = new int[NUM_SENS];        //for circular array
    sensorVal = new float[NUM_SENS];      //init. feat value array of pointers these will store the sensor values and pass them to the buffer
    rms = new float [NUM_SENS];           //init rms for each EMG

    
    //all indices of the circular buffers start from NULL
    for (int i = 0; i<NUM_SENS; i++) {
        myIndex [i] = NULL;
    }
    
    sampleRate = 44100;
    wavePhase = 0;
    pulsePhase = 0;
    
    ofSoundStreamSetup(2, 0, sampleRate, 512, 3);

    modality = 1;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
//    buffersize = ofMap(ofGetMouseX(), 0, ofGetWidth(), 10, 512);
    
//    buffersize = ofMap(ofGetMouseX(), 0, ofGetWidth(), 100, buffersize);
    

    
    //    if(ofGetFrameNum()%60 == 0){
    
    for ( int i=0; i<myo.getDevices().size(); i++ ) {
        
        //get and save Accel data
        for (int a = 0; a<ACCEL; a++) {
            
            XMLAcc[a].addValue("Acc_" + ofToString(a), myo.getDevices()[i]->getAccel().getPtr()[a]);
            //XMLAcc[a].saveFile(path + "/AccData_" + ofToString(a) + ".xml");
            
        }
        
        //get and save 8 sensor data
        for (int j = 0; j<NUM_SENS; j++) {
            
            //let's fill the array with the sensor data
            sensorVal[j] = myo.getDevices()[i]->getEmgSamples()[j];
            putValInArrayIndex(sensorVal[j], buffer[j], buffersize, j);
            
            //fill the audioBuffer
            //sBuffer[j].copyFrom(buffer[j], buffersize, 1, sampleRate);
            //get the rms for each EMG sensor
            //rms[j] = sBuffer[j].getRMSAmplitude();
           
            //get the rms of the buffer
            rms[j] = getRMS(buffer[j], buffersize);
            
            //let's read each sensor value and store it in the xml file
            XML[j].addValue("S_" + ofToString(j), myo.getDevices()[i]->getEmgSamples()[j]);
            //XML[j].saveFile(path + "/sensorData_" + ofToString(j) + ".xml");
           
            
            //mesh setting
            EMGMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
            EMGMesh.addVertex(ofVec3f(myo.getDevices()[i]->getEmgSamples()[j%1],myo.getDevices()[i]->getEmgSamples()[j%2],0));
           
        }
        
        
        //        }
    }
    
//    cam.setDistance(500+myo.getDevices()[0]->getGyro().x);
    cam.setDistance(500);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    //drawbitmap the values
    drawBitmapVals(10, 10);
    ofDrawBitmapString(ofToString(sBuffer[0].getRMSAmplitude()), 10, 400);

    
    // bar chart of the sensors
    for ( int i=0; i<myo.getDevices().size(); i++ ) {
        for ( int j=0; j<NUM_SENS; j++ ) {
            //draw some bars
            ofDrawRectangle(10 + (j * (10+2)) , ofGetHeight() - 100, 10, myo.getDevices()[i]->getEmgSamples()[j]);
        }
    }
    
    
    //start camera
    cam.begin();

//    float *dataPointer, int arraylength, float xpos, float ypos,float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode
//    drawChoir(buffer[0], buffersize, ofGetWidth()/4, ofGetHeight()/4, rms[0]* 10, 10, buffersize, ofColor(100,100, 255, 200), OF_PRIMITIVE_POINTS);

    
    //mesh setting
    ofPushMatrix();
 
//    EMGMesh.draw();
//    mesh.draw();

    for ( int j=0; j<NUM_SENS; j++ ) {
        
        //float *dataPointer, int arraylength, float xpos, float ypos,float maxWl, float maxAmp, ofColor color, float linewidth
        //            drawSine(buffer[j], buffersize, 0, 300+(40*j), 3, 1.2, ofColor (255-10*j), 1);
        
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        //float *dataPointer, int arraylength, float xpos, float ypos, float zpos, float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode
        //let's try drawing some cool circle
//        drawChoir(buffer[j], buffersize, 0, 0, 0 , rms[j] * 10, 10, 10, ofColor(255/(j+1),100+rms[j], 255-j, 50), OF_PRIMITIVE_LINE_STRIP);
        drawChoir(buffer[j], buffersize, 0, 0, 0, rms[j] * 10, 10, buffersize, ofColor(255/(rms[j]+1),250, 250, 200), OF_PRIMITIVE_LINE_LOOP);
        drawChoir(buffer[0], buffersize, 0, 0, 0, rms[j] * 10, 10, buffersize , ofColor(255/(rms[j]+1),250, 250, 100), OF_PRIMITIVE_TRIANGLE_FAN);


        
//        drawChoir(buffer[j], buffersize, 0, 0, ofGetMouseX(), rms[j] * 10, 10, buffersize, ofColor(255/(j+1),100+rms[j], 255-j, 200), OF_PRIMITIVE_POINTS);
        
    }

    
    ofPopMatrix();
    
    cam.end();
    

}

void ofApp::exit(){
    myo.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //no data gets saved unless you hit the s key
    if(key == 's'){
        
        folder++;
        for (int j = 0; j<NUM_SENS; j++) {
           
            XML[j].saveFile(path+"/sensorData_" + ofToString(j) + ".xml");
            
            
        }
    }
    
    
}


//------------------------------------- DRAW sine chart -----------------------------------


void ofApp::drawSine(float *dataPointer, int arraylength, float xpos, float ypos,
                           float maxWl, float maxAmp, ofColor color, float linewidth){
    
    
    ofPushMatrix();
    float cursor = ofGetMouseX();

    ofTranslate(xpos, ypos);


    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    float amplitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0, maxAmp);
//    float wl = ofMap(ofGetMouseX(), 0, ofGetWindowWidth(), 0, maxWl);
    float wl = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, maxWl);

        for (int x=0; x<arraylength; x++) {
            cursor=x+wl;
            mesh.addVertex(ofVec2f(xpos + x*wl, dataPointer[x]*amplitude ));
            mesh.addColor(color);
            
        }

    centroid = mesh.getCentroid();
    mesh.draw();
    
    
    ofPopStyle();
    ofPopMatrix();
    ofDrawBitmapString(ofToString(wl), 200, 200);

}



//------------------------------------- DRAW Choir chart 2Vec-----------------------------------


void ofApp::drawChoir(float *dataPointer, int arraylength, float xpos, float ypos,
                     float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode){
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(mode);

    angle = TWO_PI / sides;
    theta = 0;
    halfAngle = angle/2.0+sin(theta);

    
    for (int x=0; x<arraylength; x++) {
        
            float xp = xpos  + dataPointer[x] + cos(x+angle) * radius;
            float yp = ypos  + dataPointer[x] + sin(x+angle) * radius;
        
            mesh.addVertex(ofVec2f(xp, yp ));

//            xp = xpos + cos(x+halfAngle) * innerRadius;
//            yp = ypos + sin(x+halfAngle) * innerRadius;
//            mesh.addVertex(ofVec2f(xp, yp));

            mesh.addColor(fillColor);

        
        if (x < arraylength-1) {
            mesh.addIndex(x);
        }else{
            
            mesh.addIndex(0);
        }
        //display vertex index
//            ofDrawBitmapString(ofToString(mesh.getIndex(x)), xp, yp);
        
        
    }
    
    //control rotation with Roll
    centroid = mesh.getCentroid();
    ofRotateZ(ofRadToDeg(myo.getDevices()[0]->getRoll()));
    ofRotateY(ofRadToDeg(myo.getDevices()[0]->getYaw()));
    ofRotateX(ofRadToDeg(myo.getDevices()[0]->getPitch()));


    ofTranslate(-centroid);

    mesh.draw();
    
    
    ofPopStyle();
    ofPopMatrix();
    
}

//------------------------------------- DRAW Choir chart 3Vec -----------------------------------


void ofApp::drawChoir(float *dataPointer, int arraylength, float xpos, float ypos, float zpos,
                      float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode){
    
    ofPushMatrix();
    ofTranslate(xpos, ypos);
    
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
        
        mesh.addVertex(ofVec3f(xp, yp, zpos ));
        mesh.addColor(fillColor);
        
        if (x < arraylength-1) {
            mesh.addIndex(x);
        }else{
        
            mesh.addIndex(arraylength-1);
        }
        //display vertex index
//        ofDrawBitmapString(ofToString(mesh.getIndex(x)), xp, yp);

        //            ofDrawCircle(xp,yp, radius); //

        
    }
    

    
    
    //control rotation with Roll
    centroid = mesh.getCentroid();
    ofRotateZ(ofRadToDeg(myo.getDevices()[0]->getRoll()));
    ofRotateY(ofRadToDeg(myo.getDevices()[0]->getYaw()));
    ofRotateX(ofRadToDeg(myo.getDevices()[0]->getPitch()));
    
    
    ofTranslate(-centroid);
    
    mesh.draw();
    
    
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
void ofApp::keyReleased(int key){
    
    if (key == '1') {
        modality == 1;
    }
    
    if (key == '2') {
        modality == 2;
    }
    
    
    
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

