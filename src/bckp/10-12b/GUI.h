#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"

class GUI {
    
public:
    void setup();
    void update();
    void draw();
    
    ofxDatGuiValuePlotter* plotter;
    vector<ofxDatGuiComponent*> components;
    ofxDatGuiFolder* f1;
    ofxDatGuiFolder* f2;
    
    
    
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
    
    bool bRecordTXT, bRecordXML, bSendOSC;

    
};
