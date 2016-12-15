#include "OSCmsg.h"

//--------------------------------------------------------------
void OSCmsg::setup(string name, int loopSize){
    
    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, PORT);
    

    
}

//--------------------------------------------------------------
void OSCmsg::update(float data, int loopSize, bool xml, bool txt, int samplingTime, string name){
    
    
    try {
        
        //this method attempts to send 20 frames at once, 50 times a second over OSC.
        
//        dev.read(frames); // get 20 frames from device
        ofxOscMessage m[20];
        
        for (int i = 0;i<20;i++) {
//            const BITalino::Frame &f = frames[i];  // get a reference to the first frame of each 100 frames block
            //    printf("%d : %d %d %d %d ; %d %d %d %d %d %d\n",   // dump the first frame
            //           f.seq,
            //           f.digital[0], f.digital[1], f.digital[2], f.digital[3],
            //           f.analog[0], f.analog[1], f.analog[2], f.analog[3], f.analog[4], f.analog[5]);
            m[i].setAddress("/wek/inputs");
            //m[i].addIntArg(i);
            //m[i].addIntArg(f.seq);
            //m[i].addIntArg(f.digital[0]);
            //m[i].addIntArg(f.digital[1]);
            //m[i].addIntArg(f.digital[2]);
            //m[i].addIntArg(f.digital[3]);
//            m[i].addFloatArg(f.analog[0]);
////            m[i].addFloatArg(myFilter.lopass(fabs((f.analog[0]/512)-1.),0.125));
//            m[i].addFloatArg(f.analog[1]);
//            m[i].addFloatArg(f.analog[2]);
//            m[i].addFloatArg(f.analog[3]);
//            m[i].addFloatArg(f.analog[4]);
            
            sender.sendMessage(m[i]);
//            printf("message %s\n", (frames[i]));
            
        }
    
    
    }
    catch(int e)
    {
        cout << "An exception occurred. Exception Nr. " << e << '\n';
    }
}

//--------------------------------------------------------------
void OSCmsg::draw(){
    
    
    
    
}


//--------------------------------------------------------------
void OSCmsg::keyPressed(int key){
       
}







//--------------------------------------------------------------
void OSCmsg::keyReleased(int key){
    
    
    
}

