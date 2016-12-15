#include "WriteFile.h"

//--------------------------------------------------------------
void WriteFile::setup(string name, int loopSize){
    
    path = "sensorData/" + ofToString(ofGetDay()) +"_"+ ofToString(ofGetHours())+"_"+ ofToString(ofGetMinutes())+"_"+ ofToString(ofGetSeconds());

    
    //clear the xml, just in case
//    for (int j = 0; j<loopSize; j++) {
    
        XML.clear();
//    }
    
    //create the folder if doens't exist already
//    for (int a = 0; a<loopSize; a++) {
    
        bufferAcc.begin();
        fileEMG.open(path +"/"+ name + ".txt",ofFile::Append);
//    }
    
    
}

//--------------------------------------------------------------
void WriteFile::update(float data, int loopSize, bool xml, bool txt, int samplingTime, string name){
    
    
    if(ofGetFrameNum()%samplingTime == 0){
        
        
        //get and save Accel data
//        for (int a = 0; a<loopSize; a++) {
        
            if (xml) {
                XML.addValue(name , data);
                XML.saveFile(path +"/"+ name + ".xml");
            }
            
            if (txt){
                bufferEMG.append(ofToString(floor( data*100)/100) + "\n");
                fileEMG.writeFromBuffer(bufferEMG);
                
            }
//        }
        
    }
    
}

//--------------------------------------------------------------
void WriteFile::draw(){
    
    
    
    
}


//--------------------------------------------------------------
void WriteFile::keyPressed(int key){
    
    //no data gets saved unless you hit the s key
    if(key == 's'){
        
        //create dir if doesn't exist
        ofDirectory dir(path);
        if(!dir.exists()){
            dir.create(true);
        }
        
        
    }
    
}







//--------------------------------------------------------------
void WriteFile::keyReleased(int key){
    
    
    
}

