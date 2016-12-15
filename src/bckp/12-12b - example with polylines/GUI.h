#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "myTheme.h"


#define NUM_SENS 8
#define ACCEL 3


//enum modality{ mode1,mode2,mode3,mode4 };


class GUI {
    
public:
    void setup();
    void update();
    void draw();
    
    string modality = "one";
    
    ofxDatGuiValuePlotter* plotter;
    vector<ofxDatGuiComponent*> components;

    ofxDatGuiFolder* f0;
    ofxDatGuiFolder* f1;
    ofxDatGuiFolder* f2;
    
    ofxDatGuiDropdown* dropdown;
    vector<string> options = {"one", "two", "three", "four"};

    
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void onMatrixEvent(ofxDatGuiMatrixEvent e);
    void onColorPickerEvent(ofxDatGuiColorPickerEvent e);
    void on2dPadEvent(ofxDatGui2dPadEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);
    
    
    int guiWidth;
    int x;
    int y;
    
    bool bRecordTXT, bRecordXML, bSendOSC , bActivateSound;
    
    ofxDatGuiSlider* EMGslider[NUM_SENS];
    ofxDatGuiSlider* ACCSlider[ACCEL];
    
    ofxDatGuiValuePlotter* plotterEMGRMS;
    ofxDatGuiValuePlotter* plotterACCRMS;

    
    ofxDatGui* myGui;
    myTheme myTheme;

    
};


