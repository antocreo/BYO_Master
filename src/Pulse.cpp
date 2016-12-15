#include "Pulse.h"

/*
 This class is an original implementation based on 2 examples from OF + some original code written by me. The examples are the readFile and the soundBuffer.
 From the first example I took the algorithm that read each line (in setup())
 From the soundbuffer I took the way the pulse chord is created. All the functions, the way they are passed to the ofApp, the way the sound is dilated in time,
 are implemented by me.
 
 */

//-------------------------------------------------------------- //this is the setup to initialise for reading a recorded file
void Pulse::setup(string filePath){
    
    sampleRate = 44100;
	wavePhase = 0;
	pulsePhase = 0;
    audioBufferSize = 128;
	
	// start the sound stream with a sample rate of 44100 Hz, and a buffer
	// size of 512 samples per audioOut() call
	ofSoundStreamSetup(2, 0, sampleRate, audioBufferSize, 3);
    
    nextLetterTime = ofGetElapsedTimeMillis();
    lineCount      = 0;
    letterCount    = 0;
    
    // this is our buffer to stroe the text data
    ofBuffer buffer = ofBufferFromFile(filePath);
    
    if(buffer.size()) {
        
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            
            string line = *it;
            
            // copy the line to draw later
            // make sure its not a empty line
            if(line.empty() == false) {
                seussLines.push_back(line);
            }
            
            // print out the line
//            cout << ofToString(seussLines) << endl;
            
        }
    }
    temp = 0;

}

//-------------------------------------------------------------- this setup is for real time performance
void Pulse::setup(int buffSize){
    
    sampleRate = 44100;
    wavePhase = 0;
    pulsePhase = 0;
    audioBufferSize = buffSize;
    nextTime = ofGetElapsedTimeMillis();
    
    // start the sound stream with a sample rate of 44100 Hz, and a buffer
    // size of 512 samples per audioOut() call
    ofSoundStreamSetup(2, 0, sampleRate, audioBufferSize, 3);

    
    temp = 0;
    
}

//--------------------------------------------------------------
void Pulse::update(){
	
	// This lock makes sure we don't use lastBuffer
	// from both threads simultaneously (see the corresponding lock in audioOut())
	unique_lock<mutex> lock(audioMutex);

	rms = lastBuffer.getRMSAmplitude();
}

//--------------------------------------------------------------
void Pulse::draw(){
	

}

//--------------------------------------------------------------
void Pulse::audioOut(ofSoundBuffer &outBuffer, float multiplier) {
	
    
    float time = ofGetElapsedTimeMillis() - nextLetterTime;
    
    if(time > 100) {
        if (seussLines.size()>0) {
            frequency = ((ofToFloat(seussLines[temp])) * multiplier) ;
            nextLetterTime = ofGetElapsedTimeMillis();
            temp++;
        }
    }
  
	// base frequency of the lowest sine wave in cycles per second (hertz)
	
	// mapping frequencies from Hz into full oscillations of sin() (two pi)
	float wavePhaseStep = (frequency / sampleRate) * TWO_PI;
	float pulsePhaseStep = (0.5 / sampleRate) * TWO_PI;
	
	// this loop builds a buffer of audio containing 3 sine waves at different
	// frequencies, and pulses the volume of each sine wave individually. In
	// other words, 3 oscillators and 3 LFOs.
	
	for(size_t i = 0; i < outBuffer.getNumFrames(); i++) {
		
		// build up a chord out of sine waves at 3 different frequencies
		float sampleLow = sin(wavePhase);
		float sampleMid = sin(wavePhase * 1.5);
		float sampleHi = sin(wavePhase * 2.0);
		
		// pulse each sample's volume
		sampleLow *= sin(pulsePhase);
		sampleMid *= sin(pulsePhase * 1.04);
		sampleHi *= sin(pulsePhase * 1.09);
		
		float fullSample = (sampleLow + sampleMid + sampleHi);
		
		// reduce the full sample's volume so it doesn't exceed 1
		fullSample *= 0.3;
		
		// write the computed sample to the left and right channels
		outBuffer.getSample(i, 0) = fullSample;
		outBuffer.getSample(i, 1) = fullSample;
		
		// get the two phase variables ready for the next sample
		wavePhase += wavePhaseStep;
		pulsePhase += pulsePhaseStep;
	}
	
	unique_lock<mutex> lock(audioMutex);
	lastBuffer = outBuffer;
    

}

//--------------------------------------------------------------
void Pulse::audioOut(ofSoundBuffer &outBuffer, float tTime, float tFrequency, float multiplier, float m1, float m2, float m3) {
    
    //parameters of the function are:
    //float tTime, float frequency, float multiplier
    /*
     the time is used to control how many values per cycle the buffer outbuffer will read. So for instance, if the time is 100, every 100ms the buffer will read one value
     this allows control on the melody, mostly. The longer the time, the more neat is the sound of each frequency. It is in other words, the distance between a note and the other.
     The frequency var is the fundamental frequency and the multiplier is the multiplier of that frequency.
     the vars m1, m2, m3 are the fundamental frequencies of the harmonics
     */
    
    //this is my implementation to control the stream of data
    float time = ofGetElapsedTimeMillis() - nextTime;
    
    if(time > tTime) {
        if (lastBuffer.size()>0) {
            frequency = (tFrequency * multiplier) ;
            nextTime = ofGetElapsedTimeMillis();
            temp++;
        }
        if (temp == lastBuffer.size()) {
            temp==0;
        }
    }
    
    // base frequency of the lowest sine wave in cycles per second (hertz)          //this code is based on the OF example
    
    // mapping frequencies from Hz into full oscillations of sin() (two pi)
    float wavePhaseStep = (frequency / sampleRate) * TWO_PI;
    float pulsePhaseStep = (0.5 / sampleRate) * TWO_PI;
    
    // this loop builds a buffer of audio containing 3 sine waves at different
    // frequencies, and pulses the volume of each sine wave individually. In
    // other words, 3 oscillators and 3 LFOs.
    
    for(size_t i = 0; i < outBuffer.getNumFrames(); i++) {
        
        // build up a chord out of sine waves at 3 different frequencies
        float sampleLow = sin(wavePhase * m1);
        float sampleMid = sin(wavePhase * m2);  //1.5
        float sampleHi = sin(wavePhase * m3);   //2.0
        
        // pulse each sample's volume
        sampleLow *= sin(pulsePhase);
        sampleMid *= sin(pulsePhase * 1.04);
        sampleHi *= sin(pulsePhase * 1.09);
        
        float fullSample = (sampleLow + sampleMid + sampleHi);
        
        // reduce the full sample's volume so it doesn't exceed 1
        fullSample *= 0.3;
        
        // write the computed sample to the left and right channels
        outBuffer.getSample(i, 0) = fullSample;
        outBuffer.getSample(i, 1) = fullSample;
        
        // get the two phase variables ready for the next sample
        wavePhase += wavePhaseStep;
        pulsePhase += pulsePhaseStep;
    }
    
    unique_lock<mutex> lock(audioMutex);
    lastBuffer = outBuffer;                                             //end of the OF example
    
    
}




//--------------------------------------------------------------
void Pulse::keyPressed(int key){

}

//--------------------------------------------------------------
void Pulse::keyReleased(int key){

}

//--------------------------------------------------------------
void Pulse::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void Pulse::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void Pulse::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void Pulse::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void Pulse::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void Pulse::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void Pulse::windowResized(int w, int h){

}

//--------------------------------------------------------------
void Pulse::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void Pulse::dragEvent(ofDragInfo dragInfo){ 

}
