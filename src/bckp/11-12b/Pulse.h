#pragma once

#include "ofMain.h"

class Pulse {

	public:
		void setup(int buffSize);
        void setup(string filePath);
		void update();
		void draw();
	
		void audioOut(ofSoundBuffer &outBuffer, float tTime, float frequency, float multiplier);
        void audioOut(ofSoundBuffer &outBuffer, float multiplier);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
		double wavePhase;
		double pulsePhase;
		double sampleRate;
        int audioBufferSize;
	
		mutex audioMutex;
		ofSoundBuffer lastBuffer;
		ofPolyline waveform;
		float rms;
    
    
    float           nextLetterTime, nextTime;
    int             lineCount;
    int             letterCount;
    vector <string> seussLines;
    float frequency;
    int temp;
};
