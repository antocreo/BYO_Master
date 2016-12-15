#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //some general setup
    ofSetVerticalSync(true);
    //    cam.setDistance(100);
    cam.setPosition(0, 0, 500);
    
    ofSetCircleResolution(64);
    ofBackground(0);
    
    //--------setup myo
    myo.setup();
    
    
    //define the buffer. I set 512 to be used for some future app with the audio but can be anything atm
    buffersize = 100;
    
    //initialise the buffer array of pointers with array of floats of size buffersize (it's an array of array basically)
    //this will store the stream of values ccoming from each sensor
    for (int j = 0; j<NUM_SENS; j++) {
        buffer[j] = new float[buffersize];                           //main buffer
        centroid[j] = 0;
        bufSens[j] = 0;                                              //array used to pass the OSC data
        sensorsFile[j].setup("sensor" + ofToString(j), NUM_SENS);    //setup the file record
        
    }
    for (int a = 0; a<ACCEL; a++) {
        bufferAcc[a] = new float[buffersize];                       //main buffer
        bufAcc[a] = 0;                                              //array used to pass the OSC data
        accelFile[a].setup("accel" + ofToString(a), ACCEL);         //setup the file record
        
    }
    
    myIndex = new int[NUM_SENS];            //for circular array
    sensorVal = new float[NUM_SENS];        //init. feat value array of pointers these will store the sensor values and pass them to the buffer
    accVal = new float[ACCEL];              //init. feat value array of pointers these will store the Accell. values and pass them to the buffer
    rmsSens = new float [NUM_SENS];             //init rms for each EMG
    rmsAcc = new float [ACCEL];             //init rms for each ACC
    avrgRMSAcc = avrgRMSSens = 0;
    
    //all indices of the circular buffers start from NULL
    for (int i = 0; i<NUM_SENS; i++) {
        myIndex [i] = NULL;
    }
    
    sampleRate = 44100;
    wavePhase = 0;
    pulsePhase = 0;
    
    //-----setup OSC
    sender.setup(HOST, PORT);
    
    //-----ofxBullet
    
    world.setup();
    // enables mouse Pick events //
    world.enableGrabbing();
    ofAddListener(world.MOUSE_PICK_EVENT, this, &ofApp::mousePickEvent);
    
    world.enableCollisionEvents();
    ofAddListener(world.COLLISION_EVENT, this, &ofApp::onCollision);
    world.setCamera(&cam);
    world.setGravity( ofVec3f(0, 25., 0) );
    
    // set quaternion, so we know which way is up //
    ofQuaternion groundQuat;
    groundQuat.set(0, 1, 0, 1.);
    
    
    ofVec3f startLoc;
    ofPoint dimens;
    boundsWidth = 30.;
    float hwidth = boundsWidth*.5;
    float depth = 2.;
    float hdepth = depth*.5;
    startLoc.set( 0., 0., 0. );
    dimens.set(100, 5, 100);
    
    for(int i = 0; i < NUM_SENS; i++) {
        bounds.push_back( new ofxBulletBox() );
        bounds[i]->create( world.world, ofVec3f(-300.+(100*i), 0., 0.), groundQuat, 0., 100.f, 100.f, 10.f );
        bounds[i]->setProperties(.25, .95);
        bounds[i]->add();
        bounds[i]->enableKinematic();
        
    }
    
    //---- GUI setup
    gui.setup();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    

    //        buffersize = ofClamp(ofMap(ofGetMouseX(), 0, ofGetWidth(), 5, 100),5,100);
    
    if (myo.getDevices().size()>0) {
        
        
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            
            //get and save Accel data
            for (int a = 0; a<ACCEL; a++) {
                //---------------let's fill the array with the accelerometer data
                
                accVal[a] = myo.getDevices()[i]->getAccel().getPtr()[a];            //fill the accVal
                putValInArrayIndex(accVal[a], bufferAcc[a], buffersize, a);
                
                //--------------- get the rms of the bufferACC
                rmsAcc[a] = getRMS(bufferAcc[a], buffersize);
                avrgRMSAcc = getRMS(rmsAcc, ACCEL);
                
                //save the file
                accelFile[a].update(myo.getDevices()[i]->getAccel().getPtr()[a], ACCEL, gui.bRecordXML, gui.bRecordTXT, 60, "Accel" + ofToString(a));
                
                //-------------- OSC message for the ACC
                if (gui.bSendOSC) {
                    bufAcc[a] = accVal[a];
                    oscAcc[a].setAddress("/ACC/");
                    oscAcc[a].addFloatArg(bufAcc[a]);
                    sender.sendMessage(oscAcc[a]);
                    
                }
                
                //------------ udpate GUI
                float mapVal[ACCEL];
                mapVal[a] = myo.getDevices()[0]->getEmgSamples()[a];
                gui.ACCSlider[a]->bind(mapVal[a], 0, 100);
                gui.ACCSlider[a]->setBackgroundColor(ofColor(50*rmsAcc[a],100,100));
                gui.ACCSlider[a]->setStripeColor(ofColor(0,240,0));
                
            }
            
            //get and save 8 sensor data
            for (int j = 0; j<NUM_SENS; j++) {
                
                //--------------- let's fill the array with the sensor data
                sensorVal[j] = myo.getDevices()[i]->getEmgSamples()[j];
                putValInArrayIndex(sensorVal[j], buffer[j], buffersize, j);
                
                //--------------- fill the audioBuffer
                //sBuffer[j].copyFrom(buffer[j], buffersize, 1, sampleRate);
                //get the rms for each EMG sensor
                //rms[j] = sBuffer[j].getRMSAmplitude();
                
                //--------------- get the rms of the buffer
                rmsSens[j] = getRMS(buffer[j], buffersize);
                avrgRMSSens = getRMS(rmsSens, NUM_SENS);
                
                
                //--------------- Recording File
                //save file if pressed 's' - with this class you can choose wheter saving an XML and/or a TXT file
                sensorsFile[j].update(sensorVal[j], NUM_SENS, gui.bRecordXML,gui.bRecordTXT, 60, "Sens"+ ofToString(j));
                
                //-------------- OSC message for the SENSORS
                if (gui.bSendOSC) {
                    bufSens[j] = sensorVal[j];
                    oscSens[j].setAddress("/EMG/");
                    oscSens[j].addFloatArg(bufSens[j]);
                    sender.sendMessage(oscSens[j]);
                }
                
                //------------ udpate GUI
                float mapVal[NUM_SENS];
                mapVal[j] = myo.getDevices()[0]->getEmgSamples()[j];
                gui.EMGslider[j]->bind(mapVal[j], 0, 100);
                gui.EMGslider[j]->setBackgroundColor(ofColor(30*rmsSens[j],100,100));
                gui.EMGslider[j]->setStripeColor(ofColor(240,0,0));
                //RMS plotter
                gui.plotterEMGRMS->setValue(avrgRMSSens);
                //                gui.plotterEMGRMS->setRange(-20, 20);
                gui.plotterEMGRMS->setSpeed(10);
                gui.plotterEMGRMS->setDrawMode(ofxDatGuiGraph::LINES);
                
            }
        }
        
        //---------- cam setting------------//
        
        //            cam.setOrientation(myo.getDevices()[0]->getQuaternion());
        
        
        
        //ofxBullet
        for(int i = 0; i < bounds.size()-1; i++) {
            // store the position of the ground //
            ofVec3f pos = bounds[i]->getPosition();
            
            // create a bullet transform object and set the position to that of the object //
            btTransform trans;
            trans.setOrigin( btVector3( btScalar(pos.x), btScalar(pos.y), btScalar(pos.z)) );
            
            
            // get the rotation quaternion from the ground //
            ofQuaternion rotQuat = bounds[i]->getRotationQuat();
            float newRot = rotQuat.w();
            // rotate it a bit //
            newRot += .01f;
            // clamp values between PI and -PI so there is no jumping //
            if(newRot >= PI) newRot -= PI;
            if(newRot <= 0) newRot += PI;
            
            // set the rotation of the bullet transform to that of the axis of the stored quaternion
            // and apply the new rotation
            trans.setRotation( btQuaternion(btVector3(myo.getDevices()[0]->getPitch(), myo.getDevices()[0]->getYaw(), myo.getDevices()[0]->getRoll()), newRot) );
            
            // apply the transform to the rigid body //
            bounds[i]->getRigidBody()->getMotionState()->setWorldTransform( trans );
            // tell the ofxBulletWorldRigid that we have moved rigid body and it should update the collision object //
            bounds[i]->activate();
            //        // print out the angle
            //        cout << "rotation " << rotQuat.w() << endl;
            
            
        }
        
        
        world.update();
        
    } // end of if myo is found
    
    
    // -------- GUI update
    
    gui.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    if (myo.getDevices().size()>0) {
        
        
        //drawbitmap the values
        drawBitmapVals(10, 10);
        ofDrawBitmapString("RMS ACC " + ofToString(avrgRMSAcc), 10, 380);
        ofDrawBitmapString("RMS SENS " + ofToString(avrgRMSSens), 10, 400);
        
        ofDrawBitmapString("Roll " + ofToString(ofRadToDeg(myo.getDevices()[0]->getRoll())), 10, 420);
        ofDrawBitmapString("Yaw " + ofToString(ofRadToDeg(myo.getDevices()[0]->getYaw())), 10, 440);
        ofDrawBitmapString("Pitch " + ofToString(ofRadToDeg(-myo.getDevices()[0]->getPitch())), 10, 460);
        ofDrawBitmapString("Modality " + gui.modality, 10, 480);
        
        
        // bar chart of the sensors
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            for ( int j=0; j<NUM_SENS; j++ ) {
                //draw some bars
                ofDrawRectangle(10 + (j * (10+2)) , ofGetHeight() - 100, 10, myo.getDevices()[i]->getEmgSamples()[j]);
            }
        }
        
        //----modality 3 simple sine graph
        
        if (gui.modality == "one") {
            for ( int j=0; j<NUM_SENS; j++ ) {
                
                float xpos = 100;
                float ypos = 200;
                float gap = 40;
                float maxWl = 5;
                float maxAmp = 1.2;
                //float *dataPointer, int arraylength, float xpos, float ypos,float maxWl, float maxAmp, ofColor color
                drawSine(buffer[j], buffersize, xpos, ypos+(gap*j), maxWl, maxAmp, ofColor (10*(rmsSens[j]),100,20));
            }
        }
        
        
        //start camera
        cam.begin();
        
        //mesh setting
        ofPushMatrix();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        
        for ( int j=0; j<NUM_SENS; j++ ) {
            
            
            //float *dataPointer, int arraylength, float xpos, float ypos, float zpos, float yMultiplier, float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode
            //let's try drawing some cool circle
            
            //----modality 1 point cloud
            if (gui.modality == "two") {
                float k = 2;
                drawChoir2(buffer[j], buffersize,  0, 0, 0, 2*rmsSens[j], 10*rmsSens[j], buffersize, ofColor(255/(j+1),100+rmsSens[j], 255-j, 200), OF_PRIMITIVE_POINTS);
                drawChoir2(buffer[j], buffersize,  0, 0, 0, 2*rmsSens[j], 10*rmsSens[j], buffersize, ofColor(255/(j+1),100+rmsSens[j], 255-j, 10*rmsSens[j]), OF_PRIMITIVE_LINE_LOOP);
            }
            
            //----modality 2 circular polygon
            if (gui.modality == "three"){
                drawChoir(buffer[j], buffersize, 0, 0, 0, rmsSens[j] * 10, buffersize, ofColor(255/(j+1),200-rmsSens[j], 100+j, 200), OF_PRIMITIVE_LINE_LOOP);
                drawChoir(buffer[j], buffersize, 0, 0, 0, rmsSens[j] * 10, buffersize , ofColor(255/(j+1),200-rmsSens[j], 100+j, 4*rmsSens[j]), OF_PRIMITIVE_TRIANGLE_STRIP);
            }
        }
        
        //ofxBullet
        if (gui.modality == "four"){
            
            ofSetColor(100, 100, 100);
            for(int i = 0; i < bounds.size()-1; i++) {
                bounds[i]->draw();
            }
        }
        
        ofDisableBlendMode();
        ofPopMatrix();
        
        cam.end();
        
        
    } //end of find MYO
    
    gui.draw();
    
    
}

//------------------------------------- EXIT -----------------------------------


void ofApp::exit(){
    myo.stop();
}



//------------------------------------- DRAW sine chart -----------------------------------


void ofApp::drawSine(float *dataPointer, int arraylength, float xpos, float ypos,
                     float maxWl, float maxAmp, ofColor color){
    
    
    ofPushMatrix();
    ofTranslate(xpos, ypos);
    
    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    float amplitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), 1, maxAmp);
    float wl =  maxWl;
    
    for (int x=0; x<arraylength; x++) {
        mesh.addVertex(ofVec2f(xpos + x*wl, dataPointer[x]*amplitude ));
        mesh.addColor(color);
        
    }
    
    ofVec3f thisCentroid = mesh.getCentroid();
    mesh.draw();
    
    
    ofPopStyle();
    ofPopMatrix();
    //    ofDrawBitmapString(ofToString(wl), 200, 200);
    
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
        //        float xp = xpos + cos(x*angle) * (radius + dataPointer[x]);
        //        float yp = ypos + sin(x*angle) * (radius + dataPointer[x]);
        //        float zp = zpos ;
        
        float xp = xpos + cos(x*angle)*radius;
        float yp = ypos + sin(x*angle)*radius;
        float zp = zpos + cos(x*angle) * (dataPointer[x]);
        
        
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
    
    
    //    double acc = 0;
    //    for(int i = 0; i < myBufferSize; i++){
    //        acc += myBuffer[i] * myBuffer[i];
    //    }
    //    return sqrt(acc / (double)myBufferSize);
    
    float acc = 0;
    for(int i = 0; i < myBufferSize; i++){
        acc += pow(myBuffer[i], 2);
    }
    return sqrt(acc / myBufferSize);
    
    
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
    
    //    sensorsFile.keyPressed(key);
    
    if (key== 's') {
        
        
    }
    
    //    modality = key - 48;
    
    if (key== '1') {
        gui.modality = "one";
    }
    if (key== '2') {
        gui.modality = "two";
    }
    if (key== '3') {
        gui.modality = "three";
    }
    if (key== '4') {
        gui.modality = "four";
    }
    
    
    if (key== 'o') {
        gui.bSendOSC = !gui.bSendOSC;
    }
    
    
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
float ofApp::lopass(float *input, float cutoff) {
    float output = 0;
    output = input[0] + cutoff*(input[0]);
    return(output);
}

//--------------------------------------------------------------
void ofApp::onCollision(ofxBulletCollisionData& cdata) {
    for(int j = 0; j < bounds.size(); j++) {
        if(*bounds[j] == cdata) {
            return;
        }
    }
    
    for (int i = 0; i < shapes.size(); i++) {
        if(*shapes[i] == cdata) {
            bColliding[i] = true;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePickEvent( ofxBulletMousePickEvent &e ) {
    mousePickIndex = -1;
    for(int i = 0; i < shapes.size(); i++) {
        if(*shapes[i] == e) {
            mousePickIndex = i;
            mousePickPos = e.pickPosWorld;
            break;
        }
    }
}

