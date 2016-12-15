#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //some general setup
    ofSetFullscreen(true);
    ofSetVerticalSync(true);
    //    cam.setDistance(100);
    cam.setPosition(0, 0, 500);
    
    ofSetCircleResolution(64);
    ofBackground(0);
    
    //--------setup myo
    myo.setup();
    
    //define the buffer. I set 512 to be used for some future app with the audio but can be anything atm
    buffersize = MYBUFSIZE;
    
    //initialise the arrays etc.
    myIndex1 = new int[NUM_SENS];            //for circular array
    myIndex2 = new int[NUM_SENS];            //for circular array
    myIndex3 = new int[NUM_SENS];            //for circular array
    myIndexAccOSC = new int [ACCEL];
    myIndexSensOSC = new int [NUM_SENS];
    
    sensorVal = new float[NUM_SENS];        //init. feat value array of pointers these will store the sensor values and pass them to the buffer
    accVal = new float[ACCEL];              //init. feat value array of pointers these will store the Accell. values and pass them to the buffer
    rmsSens = new float [NUM_SENS];             //init rms for each EMG
    rmsAcc = new float [ACCEL];             //init rms for each ACC
    avrgRMSAcc = avrgRMSSens = 0;
    
    //initialise the buffer array of pointers with array of floats of size buffersize (it's an array of array basically)
    //this will store the stream of values ccoming from each sensor
    for (int j = 0; j<NUM_SENS; j++) {
        buffer[j] = new float[buffersize];                           //main buffer
        centroid[j] = 0;
        bufSens[j] = 0;                                              //array used to pass the OSC data
        sensorsFile[j].setup("sensor" + ofToString(j), NUM_SENS);    //setup the file record
        
        //    all indices of the circular buffers start from NULL
        myIndex1[j] = NULL;
        myIndex2[j] = NULL;
        myIndex3[j] = NULL;
        myIndexSensOSC[j] = NULL;
        
        
        //------- setting audio
        
        //initialise the pulse
        //        myPulse[j].setup(buffersize);
    }
    
    for (int a = 0; a<ACCEL; a++) {
        bufferAcc[a] = new float[buffersize];                       //main buffer
        bufAcc[a] = 0;                                              //array used to pass the OSC data
        accelFile[a].setup("accel" + ofToString(a), ACCEL);         //setup the file record
        
        //    all indices of the circular buffers start from NULL
        myIndexAccOSC[a] = NULL;
    }
    
    
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
    
    // ----- audio setup
    myPulse1.setup(buffersize);
    //    myPulse2.setup(buffersize);
    
    
    //boolean vars
    bDebug = false;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //            buffersize = ofClamp(ofMap(ofGetMouseX(), 0, ofGetWidth(), 5, 100),8,100);
    
    //I update the width and the height in these variables as it gives me problem while updating the mesh.
    //apparently if I pass ofgetW and ofgetH directly in the drawSine() function in the draw() function there is a lag
    //when passing in fullscreen
    
    widthUpdate = ofGetWidth();
    heigthUpdate = ofGetHeight();
    
    if (myo.getDevices().size()>0) {
        
        
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            
            //get and save Accel data
            for (int a = 0; a<ACCEL; a++) {
                //---------------let's fill the array with the accelerometer data
                
                accVal[a] = myo.getDevices()[i]->getAccel().getPtr()[a];            //fill the accVal
                putValInArrayIndex(accVal[a], bufferAcc[a], buffersize, a, myIndex1);
                
                //--------------- get the rms of the bufferACC
                rmsAcc[a] = getRMS(bufferAcc[a], buffersize);
                avrgRMSAcc = getRMS(rmsAcc, ACCEL);
                
                //save the file
                accelFile[a].update(myo.getDevices()[i]->getAccel().getPtr()[a], ACCEL, gui.bRecordXML, gui.bRecordTXT, 60, "Accel" + ofToString(a));
                //
                //               //-------------- OSC message for the ACC
                //                    bufAcc[a] = bufferAcc[a][buffersize-1];
                oscAcc[a].setAddress("/ACC/"+ ofToString(a)+"/");
                if (myIndex1[a]!=0) {
                    lastAcc = bufferAcc[a][myIndex1[a]];
                    oscAcc[a].addFloatArg(lastAcc);
                } else {
                    lastAcc = bufferAcc[a][buffersize-1];
                    oscAcc[a].addFloatArg(lastAcc);
                }
                if (gui.bSendOSC) {
                    sender.sendMessage(oscAcc[a]);
                }
                
                //------------ udpate GUI
                float mapVal[ACCEL];
                mapVal[a] = myo.getDevices()[0]->getAccel()[a];
                gui.ACCSlider[a]->bind(mapVal[a], 0, 1);
                gui.ACCSlider[a]->setBackgroundColor(ofColor(200*rmsAcc[a],120 - rmsAcc[a],30));
                gui.ACCSlider[a]->setStripeColor(ofColor(0,240,0));
                
                //RMS plotter
                float t=0;
                float v1 = (sin(t+=avrgRMSAcc))/2;
                gui.plotterACCRMS->setRange(0, 20);
                gui.plotterACCRMS->setValue(v1 * gui.plotterACCRMS->getRange());
                gui.plotterACCRMS->setSpeed(10);
                gui.plotterEMGRMS->setDrawMode(ofxDatGuiGraph::LINES);
                
            }
            
            //get and save 8 sensor data
            for (int j = 0; j<NUM_SENS; j++) {
                
                //--------------- let's fill the array with the sensor data
                sensorVal[j] = myo.getDevices()[i]->getEmgSamples()[j];
                putValInArrayIndex(sensorVal[j], buffer[j], buffersize, j,myIndex2);
                
                //--------------- get the rms of the buffer
                rmsSens[j] = getRMS(buffer[j], buffersize);
                avrgRMSSens = getRMS(rmsSens, NUM_SENS);
                
                
                //--------------- Recording File
                //save file if pressed 's' - with this class you can choose wheter saving an XML and/or a TXT file
                sensorsFile[j].update(sensorVal[j], NUM_SENS, gui.bRecordXML,gui.bRecordTXT, 60, "Sens"+ ofToString(j));
                
                //                //-------------- OSC message for the SENSORS
                //                    bufSens[j] = buffer[j][buffersize-1];
                oscSens[j].setAddress("/EMG/"+ ofToString(j)+"/");
                if (myIndex2[j]!=0) {
                    lastSens = buffer[j][myIndex2[j]];
                    oscSens[j].addFloatArg(lastSens);
                } else {
                    lastSens = buffer[j][buffersize-1];
                    oscSens[j].addFloatArg(lastSens);
                }
                if (gui.bSendOSC) {
                    sender.sendMessage(oscSens[j]);
//                    cout << oscSens[j].getAddress() << lastSens << endl;
                    
                }
                
                //------------ udpate GUI
                //EMG
                float mapVal[NUM_SENS];
                mapVal[j] = myo.getDevices()[0]->getEmgSamples()[j];
                gui.EMGslider[j]->bind(mapVal[j], 0, 100);
                gui.EMGslider[j]->setBackgroundColor(ofColor(30*rmsSens[j],100,100));
                gui.EMGslider[j]->setStripeColor(ofColor(240,0,0));
                //RMS plotter
                float a=0;
                float v1 = (sin(a+=avrgRMSSens))/2;
                gui.plotterEMGRMS->setRange(0, 20);
                gui.plotterEMGRMS->setValue(v1 * gui.plotterEMGRMS->getRange());
                gui.plotterEMGRMS->setSpeed(10);
                gui.plotterEMGRMS->setDrawMode(ofxDatGuiGraph::LINES);
                
                //-----setupAudio
                
                //                myPulse[i].update();
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
    
    myPulse1.update();
    //    myPulse2.update();
    
    
    // -------- GUI update
    
    gui.update();
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    if (myo.getDevices().size()>0) {
        
        if (bDebug) {
            //debug values the values
            drawBitmapVals(10, 10);
        }
        
        // bar chart of the sensors
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            for ( int j=0; j<NUM_SENS; j++ ) {
                //draw some bars
                ofDrawRectangle(10 + (j * (10+2)) , ofGetHeight() - 100, 10, myo.getDevices()[i]->getEmgSamples()[j]);
            }
        }
        
        //----modality 1 simple sine graph that stretcehs out like muscles
        
        if (gui.modality == "one") {
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            
            float maxWl = ofGetWidth()/(buffersize-3);
            float maxAmp = 2;
            float gap = 20+2*avrgRMSSens;
            float xpos = 0;
            float ypos = ofGetHeight() - gap * NUM_SENS ;
            for ( int j=0; j<NUM_SENS; j++ ) {
                
                //float *dataPointer, int arraylength, float xpos, float ypos,float maxWl, float maxAmp, ofColor color
                drawSine(buffer[j], buffersize, xpos, ypos+(gap*j), 0, maxWl, maxAmp, ofColor (10*(rmsSens[j]),100,20,150), OF_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY);
                drawSine(buffer[j], buffersize, xpos, ypos+(gap*j), 0, maxWl, maxAmp, ofColor (10*(rmsSens[j]),100,20), OF_PRIMITIVE_LINE_STRIP);
                
                
            }
            ofDisableBlendMode();
        }
        
        
        //start camera
        cam.begin();
        
        //mesh setting
        ofPushMatrix();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        
        for ( int j=0; j<NUM_SENS; j++ ) {
            
            
            //float *dataPointer, int arraylength, float xpos, float ypos, float zpos, float yMultiplier, float radius, float innerRadius, float sides, ofColor fillColor, ofPrimitiveMode mode
            //let's try drawing some cool circle
            
            //----modality 2 point cloud
            if (gui.modality == "two") {
                float k = 2;
                drawChoir2(buffer[j], buffersize,  0, 0, 0, 2*rmsSens[j], 10*rmsSens[j], buffersize, ofColor(255/(j+1),100+rmsSens[j], 255-j, 1*rmsSens[j]), OF_PRIMITIVE_TRIANGLE_FAN);
                drawChoir2(buffer[j], buffersize,  0, 0, 0, 2*rmsSens[j], 10*rmsSens[j], buffersize, ofColor(255/(j+1),100+rmsSens[j], 255-j, 200), OF_PRIMITIVE_POINTS);
                drawChoir2(buffer[j], buffersize,  0, 0, 0, 2*rmsSens[j], 10*rmsSens[j], buffersize, ofColor(255/(j+1),100+rmsSens[j], 255-j, 40*rmsSens[j]), OF_PRIMITIVE_LINE_STRIP);
                
                
            }
            
            //----modality 3 circular polygon
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

//a simple graph kind of visualisation
//note: when in fullscreen the mesh is not responsive as it should be

void ofApp::drawSine(float *dataPointer, int arraylength, float xpos, float ypos, float zpos,
                     float maxWl, float maxAmp, ofColor color, ofPrimitiveMode mode){
    
    
    ofPushMatrix();
    ofTranslate(xpos, ypos, zpos);
    
    ofPushStyle();
    ofVboMesh mesh;
    mesh.setMode(mode);
    
    //    float amplitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), 1, maxAmp);
    float amplitude =  maxAmp;
    float wl =  maxWl;
    
    for (int x=0; x<arraylength; x++) {
        if (x==0) {
            mesh.addVertex(ofVec2f(xpos + x*wl, (dataPointer[0]*amplitude) ));
            mesh.addIndex(0);
        }
        
        mesh.addVertex(ofVec2f(xpos + x*wl, (dataPointer[x]*amplitude) ));
        mesh.addColor(color);
        
        if (x<arraylength-1) {
            mesh.addIndex(x);
        } else {
            ofVec2f bottomRx(widthUpdate - xpos , heigthUpdate - ypos);
            mesh.addVertex(bottomRx);
            mesh.addIndex(arraylength);
            
            mesh.addVertex(ofVec2f(xpos , heigthUpdate - ypos));
            mesh.addIndex(arraylength+1);
            
            mesh.addVertex(ofVec2f(xpos , ypos));
            mesh.addIndex(arraylength+2);
        }
    }
    
    mesh.addColor(color);
    
    ofVec3f thisCentroid = mesh.getCentroid();
    mesh.draw();
    
    // ---------------------start of poly
  /*
    //test with polylines
        ofFill();
        ofSetColor(color);
        ofSetPolyMode(OF_POLY_WINDING_NONZERO);
        ofPolyline poly;
        ofPath path;
        path.setMode(ofPath::POLYLINES);
        for (int x=0; x<arraylength; x++) {
                poly.curveTo(mesh.getVertex(x));
            }
    
    
        poly.close();
    
        path.simplify();
        path.close();
        path.draw(xpos,ypos);
        poly.draw();
    */
    //-------------------- end of poly
    
    
    //crashes tassellation here.... no solution yet
    
    /*
        ofBeginShape();
        for( int i = 0; i < poly.getVertices().size(); i++) {
            ofVertex(poly.getVertices().at(i).x, poly.getVertices().at(i).y);
        }
        ofEndShape();
    */
    
    
    //crashes tassellation here.... no solution yet
    /*
        float amplitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), 1, maxAmp);
        float wl =  maxWl;
    
        ofBeginShape();
        for (int x=0; x<arraylength; x++) {
            if (x == 0){
    
            ofCurveVertex(xpos + x*wl, dataPointer[0]*amplitude );
            ofCurveVertex(xpos + x*wl, dataPointer[0]*amplitude );
            } else if (x == arraylength-1){
                ofVec2f bottomRx(ofGetWidth() - xpos , ofGetHeight() - ypos);
                ofCurveVertex(bottomRx.x,bottomRx.y);
                ofCurveVertex(xpos , ofGetHeight() - ypos);
            }else{
                ofCurveVertex(xpos + x*wl, dataPointer[x]*amplitude );
                }
    
        }
        ofEndShape();
    */
    
    
    ofPopStyle();
    ofPopMatrix();
    
}



//------------------------------------- DRAW Choir2 chart -----------------------------------

//a different kind of visualisation

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
    ofRotateX(ofRadToDeg(-myo.getDevices()[0]->getPitch()));
    ofVec3f thisCentroid = mesh.getCentroid();
    
    ofTranslate(thisCentroid);
    
    mesh.draw();
    
    ofPopStyle();
    ofPopMatrix();
    
}

//------------------------------------- DRAW Choir chart  -----------------------------------

//a different kind of visualisation
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
        //this version is oriented on the Y so it's flat
        //        float xp = xpos + cos(x*angle) * (radius + dataPointer[x]);
        //        float yp = ypos + sin(x*angle) * (radius + dataPointer[x]);
        //        float zp = zpos ;
        
        //this version is oriented towards Z
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
        //ofDrawBitmapString(ofToString(mesh.getIndex(x)), xp, yp);
        
    }
    
    
    //control rotation with Roll
    ofRotateZ(ofRadToDeg(myo.getDevices()[0]->getRoll()));
    ofRotateY(ofRadToDeg(myo.getDevices()[0]->getYaw()));
    ofRotateX(ofRadToDeg(-myo.getDevices()[0]->getPitch()));
    
    ofVec3f thisCentroid = mesh.getCentroid();
    
    ofTranslate(thisCentroid);
    mesh.draw();
    
    
    /* //test with polylines crashes. 
       //I think is because it has problem taking values continuosly updated like a stream.
     
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
     path.draw(xpos, ypos);
     poly.draw();
     
     */
    
    ofPopStyle();
    ofPopMatrix();
    
}

//------------------------------------- put val in array -----------------------------------

//this is a circular buffer. it takes a value and it puts it into the destination Array

void ofApp::putValInArrayIndex(float value, float *destinationArray, int arraylength, int indexVal, int* myPointer){
    
    
    destinationArray[myPointer[indexVal]] = value;
    myPointer[indexVal]++;
    
    if (myPointer[indexVal] == arraylength) {
        myPointer[indexVal]= 0;
    }
    
}
//------------------------------------- RMS  -----------------------------------

//calculates the RMS of any assigned buffer of buffersize
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

//this is just a bunch of data for debug
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
    
    
    //other values
    ofDrawBitmapString("RMS ACC " + ofToString(avrgRMSAcc), 10, 380);
    ofDrawBitmapString("RMS SENS " + ofToString(avrgRMSSens), 10, 400);
    
    ofDrawBitmapString("Roll " + ofToString(ofRadToDeg(myo.getDevices()[0]->getRoll())), 10, 420);
    ofDrawBitmapString("Yaw " + ofToString(ofRadToDeg(myo.getDevices()[0]->getYaw())), 10, 440);
    ofDrawBitmapString("Pitch " + ofToString(ofRadToDeg(-myo.getDevices()[0]->getPitch())), 10, 460);
    ofDrawBitmapString("Gyro X " + ofToString(ofRadToDeg(myo.getDevices()[0]->getGyro().getPtr()[0])), 10, 480);
    ofDrawBitmapString("Gyro Y " + ofToString(ofRadToDeg(myo.getDevices()[0]->getGyro().getPtr()[1])), 10, 500);
    ofDrawBitmapString("Gyro Z " + ofToString(ofRadToDeg(myo.getDevices()[0]->getGyro().getPtr()[2])), 10, 520);
    //    ofDrawBitmapString("Gyro W " + ofToString(ofRadToDeg(myo.getDevices()[0]->getGyro().getPtr()[3])), 10, 540);
    
    ofDrawBitmapString("Modality " + gui.modality, 10, 560);
    
    
}


//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    
    //parameters of the function are:
    //float tTime, float frequency, float multiplier
    
    if (myo.getDevices().size()>0) {
        
        
        //MODE 1
        //this is a simple modality: the time compresses the more you stretch the muscles, the pitch and is controlled my the ACC and multiplied by the EMG

        if (gui.modality == "one" && gui.bActivateSound) {
            
            float m1 = ofClamp(ofMap(avrgRMSSens, 0, 20, 1, 3), 1, 3);
            float m2 = ofClamp(ofMap(avrgRMSAcc, 0, 10, 1, 3), 1, 3);
            float m3 = ofClamp(ofMap(lastSens, -20, 20, 1, 3), 1, 3);
            
            myPulse1.audioOut(outBuffer, 100 - avrgRMSSens,
                                         60+avrgRMSAcc,
                                         lastSens + avrgRMSSens, 1, 1.3, 1.5 );
        }
        
        //MODE 2
        //this modality uses the EMG to control the pitch the acceleration controls the time stretch and the Yaw controls the pitch multiplier
        if (gui.modality == "two" && gui.bActivateSound) {
            //            myPulse1.audioOut(outBuffer, lastSens, avrgRMSSens, avrgRMSSens * ( myo.getDevices()[0]->getGyro().getPtr()[1]));
            myPulse1.audioOut(outBuffer, 100+avrgRMSAcc + fabs(myo.getDevices()[0]->getRoll()),
                                         avrgRMSSens + fabs(myo.getDevices()[0]->getPitch()),
                                         40 + fabs(myo.getDevices()[0]->getYaw()), .2, 2.1, 3);
            
        }
        
        //MODE 3
        //this modality uses the gyroscope to control the sound
        //the more you accelerate the less the sound is sincoped, the time stretches and the single notes are played more distinte,
        //at the same time the more you accelerate the more the pitch rises
        //
        if (gui.modality == "three" && gui.bActivateSound) {
            myPulse1.audioOut(outBuffer, fabs(myo.getDevices()[0]->getGyro().getPtr()[0]) + fabs(avrgRMSSens),
                              fabs(myo.getDevices()[0]->getGyro().getPtr()[1]) - ( fabs(avrgRMSSens)+(fabs(avrgRMSAcc))),
                              60 + (avrgRMSSens + fabs(myo.getDevices()[0]->getGyro().getPtr()[2]))/(avrgRMSSens+1), 2, .2, 1);
        }
        
        
        /*
         //I am trying to create a polyphony of 8 oscillators controlled by each EMG sensor but there is some problem/
         //it seems that if I call the output in the class many times it just doesn't sound out.
         
         for ( int j=0; j<NUM_SENS; j++ ) {
         
         myPulse[0].audioOut(outBuffer, 100 * avrgRMSAcc , lastSens, avrgRMSSens);
         
         }
         
         myPulse1.audioOut(outBuffer, 100 * avrgRMSAcc, myo.getDevices()[0]->getEmgSamples()[0], avrgRMSSens);
         myPulse1.audioOut(outBuffer, 100 * avrgRMSAcc, lastSens, avrgRMSSens);
         myPulse1.audioOut(outBuffer, 100 * avrgRMSAcc, lastSens, avrgRMSSens);
         myPulse2.audioOut(outBuffer, avrgRMSAcc, lastSens, avrgRMSSens + ( myo.getDevices()[0]->getGyro().getPtr()[0]));
         
         */
        
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //    sensorsFile.keyPressed(key);
    
    if (key== 'd') {
        bDebug = !bDebug;
    }
    
    
    if (key== 'f') {
        ofToggleFullscreen();
    }
    
    
    if (key== 'a') {
        gui.bActivateSound = !gui.bActivateSound;
    }
    
    
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
    if (key == 'r') {
        for ( int i=0; i<myo.getDevices().size(); i++ ) {
            
            myo.getDevices()[i]->reset();
        }
    }
    
}

//--------------------------------------------------------------
//this is a test for a lopass filter but it is very wrong!
float ofApp::lopass(float *input, float cutoff) {
    float output = 0;
    output = input[0] + cutoff*(input[0]);
    return(output);
}

// bunch of fucntions for the bulletphysics - not implemented yet

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

