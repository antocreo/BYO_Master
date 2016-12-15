#include "Pulse.h"

//--------------------------------------------------------------
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

//--------------------------------------------------------------
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
	
	// "lastBuffer" is shared between update() and audioOut(), which are called
	// on two different threads. This lock makes sure we don't use lastBuffer
	// from both threads simultaneously (see the corresponding lock in audioOut())
	unique_lock<mutex> lock(audioMutex);

	// this loop is building up a polyline representing the audio contained in
	// the left channel of the buffer
	
	// the x coordinates are evenly spaced on a grid from 0 to the window's width
	// the y coordinates map each audio sample's range (-1 to 1) to the height
	// of the window
	
	waveform.clear();
	for(size_t i = 0; i < lastBuffer.getNumFrames(); i++) {
		float sample = lastBuffer.getSample(i, 0);
		float x = ofMap(i, 0, lastBuffer.getNumFrames(), 0, ofGetWidth());
		float y = ofMap(sample, -1, 1, 0, ofGetHeight());
		waveform.addVertex(x, y);
	}
	
	rms = lastBuffer.getRMSAmplitude();
}

//--------------------------------------------------------------
void Pulse::draw(){
	
    ofBackground(ofColor::black);
	ofSetColor(ofColor::white);
	ofSetLineWidth(1 + (rms * 30.));
	waveform.draw();
    ofDrawBitmapString(ofToString(temp), 10,100);


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
void Pulse::audioOut(ofSoundBuffer &outBuffer, float tTime, float tFrequency, float multiplier) {
    
    
    float time = ofGetElapsedTimeMillis() - nextTime;
    
    if(time > tTime) {
//        if (pointerSize>0) {
            frequency = (tFrequency * multiplier) ;
            nextTime = ofGetElapsedTimeMillis();
            temp++;
//        }
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
