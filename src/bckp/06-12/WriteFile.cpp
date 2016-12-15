#include "WriteFile.h"

//--------------------------------------------------------------
void WriteFile::setup(string name, int loopSize){
    
    path =ofToString(ofGetDay()) +"_"+ ofToString(ofGetHours())+"_"+ ofToString(ofGetMinutes())+"_"+ ofToString(ofGetSeconds());

    
    //clear the xml, just in case
    for (int j = 0; j<loopSize; j++) {
        
        XML[j].clear();
    }
    
    for (int a = 0; a<loopSize; a++) {
        
        bufferAcc[a].begin();
        fileAcc[a].open(path +"/"+ name + ofToString(a) + ".txt",ofFile::Append);
    }
    
}

//--------------------------------------------------------------
void WriteFile::update(float data, int loopSize, bool xml, bool txt, int samplingTime, string name){
    
    
    if(ofGetFrameNum()%samplingTime == 0){
        
        
        //get and save Accel data
        for (int a = 0; a<loopSize; a++) {
            
            if (xml) {
                XML[a].addValue(name + ofToString(a), data);
                XML[a].saveFile(path +"/"+ name + ofToString(a) + ".xml");
            }
            
            if (txt){
                bufferAcc[a].append(ofToString(floor( data*100)/100) + "\n");
                fileAcc[a].writeFromBuffer(bufferAcc[a]);
                
            }
        }
        
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

