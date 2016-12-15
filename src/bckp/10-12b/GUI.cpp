#include "GUI.h"

/*
    All components instantiated outside of a gui
    https://github.com/braitsch/ofxDatGui @braitsch
*/

void GUI::setup()
{
     guiWidth = 270;
     x = ofGetWidth() - guiWidth;
     y = 0;
    
//    ofSetWindowPosition(0, 0);
    
    
    f1 = new ofxDatGuiFolder("MYO", ofColor::fromHex(0xFF0000));
    f1->setWidth(guiWidth);
    f1->addFRM();
    f1->addToggle("fullscreen");
    f1->addBreak();
    f1->addToggle("send OSC");
    f1->addToggle("save XML");
    f1->addToggle("save TXT");
    f1->setPosition(x, y);
    f1->expand();
    
    f2 = new ofxDatGuiFolder("REAL TIME DATA", ofColor::fromHex(0x00FF00));
    f2->addSlider("EMG 0", 0, 100);
    f2->addSlider("EMG 1", 0, 100);
    f2->addSlider("EMG 2", 0, 100);
    f2->addSlider("EMG 3", 0, 100);
    f2->addSlider("EMG 4", 0, 100);
    f2->addSlider("EMG 5", 0, 100);
    f2->addSlider("EMG 6", 0, 100);
    f2->addSlider("EMG 7", 0, 100);
    f2->addBreak();
    f2->addSlider("ACC x", 0, 100);
    f2->addSlider("ACC Y", 0, 100);
    f2->addSlider("ACC Z", 0, 100);
    f2->addBreak();
    f2->addWaveMonitor("wave monitor", 3, .5);
    f2->addValuePlotter("plotter", ofRandom(1), ofRandom(1));
    f2->expand();

    
    
    f1->onToggleEvent(this, &GUI::onToggleEvent);
    f2->onSliderEvent(this, &GUI::onSliderEvent);
}

void GUI::update()
{
    
    x = ofGetWidth() - guiWidth;
    y = 0;

    f1->setPosition(x, y);
    f2->setPosition(x , y + f1->getHeight());

    
    f1->update();

    f2->update();
    

}

void GUI::draw()
{
    
    ofEnableAlphaBlending();
    f1->draw();
    f2->draw();
    ofDisableAlphaBlending();

}

/*
    event listeners
*/

void GUI::onButtonEvent(ofxDatGuiButtonEvent e)
{
    cout << "onButtonEvent: " << e.target->getLabel() << endl;
}

void GUI::onToggleEvent(ofxDatGuiToggleEvent e)
{
    cout << "onToggleEvent: " << e.target->getLabel() << "::" <<  e.target->getChecked() << endl;

    if (e.target->getLabel()=="send OSC") {
        bSendOSC == e.target->getEnabled();
    }
    
    if (e.target->getLabel()=="save XML") {
        bRecordXML = e.target->getEnabled();
        cout << bRecordXML << endl;
    }
    if (e.target->getLabel()=="save TXT") {
        bRecordTXT = e.target->getEnabled();
        cout << bRecordTXT << endl;

    }
    
    
}

void GUI::onSliderEvent(ofxDatGuiSliderEvent e)
{
    cout << "onSliderEvent: " << e.value << "::" << e.scale << endl;
}

void GUI::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.child << endl;
}

void GUI::onMatrixEvent(ofxDatGuiMatrixEvent e)
{
    cout << "onMatrixEvent: " << e.child << "::" << e.enabled << endl;
}

void GUI::onColorPickerEvent(ofxDatGuiColorPickerEvent e)
{
    cout << "onColorPickerEvent: " << e.color << endl;
}

void GUI::on2dPadEvent(ofxDatGui2dPadEvent e)
{
    cout << "on2dPadEvent: " << e.x << "::" << e.y << endl;
}

void GUI::onTextInputEvent(ofxDatGuiTextInputEvent e)
{
    cout << "onButtonEvent: " << e.text << endl;
}


