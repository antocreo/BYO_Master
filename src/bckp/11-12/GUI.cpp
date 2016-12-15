#include "GUI.h"

/*
    All components instantiated outside of a gui
    https://github.com/braitsch/ofxDatGui @braitsch
*/

void GUI::setup()
{
    
    bSendOSC = false;
    bRecordTXT = false;
    bRecordXML = false;
    
     guiWidth = 270;
     x = ofGetWidth() - guiWidth;
     y = 0;
    

    dropdown = new ofxDatGuiDropdown("select visualisation mode", options);
//    dropdown->expand();
    plotterEMGRMS  = new ofxDatGuiValuePlotter("EMG RMS",0,100);
    
    f1 = new ofxDatGuiFolder("SETTINGS", ofColor::fromHex(0xFF0000));
    f2 = new ofxDatGuiFolder("REAL TIME DATA", ofColor::fromHex(0x00FF00));
    
    f1->setWidth(guiWidth);
    
    f1->addFRM();
    f1->addToggle("fullscreen");
    f1->addBreak();
    f1->addToggle("send OSC");
    f1->addToggle("save XML");
    f1->addToggle("save TXT");
//    f1->setPosition(x, y);
//    f1->expand();
    
    for (int j = 0; j<NUM_SENS; j++) {
        EMGslider[j] = f2->addSlider("EMG "+ofToString(j), 0, 100);
    }
    f2->addBreak();
    ACCSlider[0] = f2->addSlider("ACC X", 0, 100);
    ACCSlider[1] = f2->addSlider("ACC Y", 0, 100);
    ACCSlider[2] = f2->addSlider("ACC Z", 0, 100);
    f2->addBreak();
    f2->addWaveMonitor("wave monitor", 3, .5);
    f2->attachItem(plotterEMGRMS);
//    f2->expand();
    
   
    
    //events
    f1->onToggleEvent(this, &GUI::onToggleEvent);
    f2->onSliderEvent(this, &GUI::onSliderEvent);
    dropdown->onDropdownEvent(this, &GUI::onDropdownEvent);
    
//    myGui->addFolder(f1);
//    myGui->addFolder(f2);
//    myGui->addDropdown("select visualusation mode", options);

}

void GUI::update()
{
    
    for (int j = 0; j<NUM_SENS; j++) {
        EMGslider[j]->update();
    }
    for (int j = 0; j<ACCEL; j++) {
        ACCSlider[j]->update();
    }
    
    x = ofGetWidth() - guiWidth;
    y = 0;
    f1->setPosition(x, y);
    f2->setPosition(x , y + f1->getY() + f1->getHeight());
    dropdown->setPosition(x, f2->getY() + f2->getHeight());
    f1->update();
    f2->update();
    dropdown->update();

    
//    myGui->update();
}

void GUI::draw()
{
    
    ofEnableAlphaBlending();
    f1->draw();
    f2->draw();
    dropdown->draw();
    
//    myGui->draw();
    ofDisableAlphaBlending();

}

/*
    event listeners
*/





void GUI::onToggleEvent(ofxDatGuiToggleEvent e)
{
//    cout << "onToggleEvent: " << e.target->getLabel() << "::" <<  e.target->getChecked() << endl;

    if (e.target->getLabel()=="send OSC") {
        bSendOSC = e.target->getChecked();
//        cout << bSendOSC << endl;

    }
    
    if (e.target->getLabel()=="save XML") {
        bRecordXML = e.target->getChecked();
//        cout << bRecordXML << endl;
    }
    if (e.target->getLabel()=="save TXT") {
        bRecordTXT = e.target->getChecked();
//        cout << bRecordTXT << endl;

    }
    if (e.target->getLabel()=="fullscreen") {
        ofToggleFullscreen();
    }

    
}

void GUI::onSliderEvent(ofxDatGuiSliderEvent e)
{
    cout << "onSliderEvent: " << e.value << "::" << e.scale << endl;
}

void GUI::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
//    cout << "onDropdownEvent: " << e.child << endl;
   
    modality = options[e.child];
    dropdown->setLabel("visualisation mode " + modality);
    
}

//void GUI::onButtonEvent(ofxDatGuiButtonEvent e)
//{
//    cout << "onButtonEvent: " << e.target->getLabel() << endl;
//}

//void GUI::onMatrixEvent(ofxDatGuiMatrixEvent e)
//{
//    cout << "onMatrixEvent: " << e.child << "::" << e.enabled << endl;
//}
//
//void GUI::onColorPickerEvent(ofxDatGuiColorPickerEvent e)
//{
//    cout << "onColorPickerEvent: " << e.color << endl;
//}
//
//void GUI::on2dPadEvent(ofxDatGui2dPadEvent e)
//{
//    cout << "on2dPadEvent: " << e.x << "::" << e.y << endl;
//}
//
//void GUI::onTextInputEvent(ofxDatGuiTextInputEvent e)
//{
//    cout << "onButtonEvent: " << e.text << endl;
//}


