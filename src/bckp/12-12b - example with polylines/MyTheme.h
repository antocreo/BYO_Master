#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"



//this is my theme to control the colors

class myTheme : public ofxDatGuiTheme{
    
public:
    
    myTheme()
    {
        stripe.visible = false;
        color.label = hex(0xF8F3F0);
        color.guiBackground = hex(0x2C3137);
        color.background = hex(0x343B41);
        color.slider.fill = hex(0xffffff);
        color.slider.text = hex(0xFFFFFF);
        color.inputAreaBackground = hex(0x434A50);
        color.textInput.text = hex(0xFFFFFF);
        color.textInput.highlight = hex(0x434A50);
        color.textInput.backgroundOnActive = hex(0x2C3137);
        color.backgroundOnMouseOver = hex(0x434A50);
        color.backgroundOnMouseDown = hex(0x2C3137);
        color.matrix.hover.button = hex(0x60B9ED);
        color.matrix.selected.button = hex(0x2C3137);
        layout.height = 30;
        init();
        
    }
    
};
