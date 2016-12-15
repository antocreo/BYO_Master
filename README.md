
# BYO
### by Anto+Creo


This is a software to control and manage real time data. In this case is bio-data using a Myo armband https://www.myo.com/
The software allows 3 types of visualisations and 3 sonic interactions.
It also allows to send data via OSC and save the session as a TXT or XML file.
Please, note that the TXT file can reach a very large size and the XML file has a size limit.
The code is supposed to work with any device that can stream data. It just need to be readapted substituting the Myo values with the respective values.

#### AIM

 The aim of this project is to experiment with bio-data in real time. I am interested in exploring ways to read real time data which is more intuitive by using, for instance, sonic or visual interaction. The visual and sonic response gives an idea of what happens in the body without having to analyse the numbers. However, having access to the numeric data is important, therefore the software is implemented to save the performance in TXT or XML file.
Once the session is recorded, it is possible to re-load the file and to rewatch the performance.
The code is prepared to do this but it is not implemented in the GUI yet.
 
 
#### PROCESS

The experience is approached as a live performance. The software allows the user to perform movements generating audio and visual outputs.
The possibility to send the bio-data via OSC can allow multiple solutions, for instance interaction with MAX patches for more complex sonic outputs, or some further visual implementation. 


#### HOW to USE IT
* step 1 - have a Myo armband, wear it and sync it.
* step 2 - open the app
* step 3 - interact with the modalities. You can change the modalities by using the keyboard or the GUI 

#### KEYBOARD COMMAND AND OPTIONS

######  Main Keyboard Commands
 ----------
 * 1              - modality 1
 * 2              - modality 2
 * 3              - modality 3
 * a              - toggle audio
 * f		      - toggle fullscreen
 * s              - save data
 * o              - send OSC

 
###### Main gestures 
 ----------
there are no specific gesture to control the audiovisual outputs, however here is some movement that you can do.
 * finger picking
 * arm tension
 * arm rotation
 * fist
 * open hand
 * just have fun with your movements!
 
#### INSTALLATION and DEPENDENCIES
you will probably need to install the myo SDK and the Myo manager software

####### OS this app is built using Mac OS X 10.9.5

####### Xcode version 6.2

####### of_v0.9.8_osx_release

It hasn’t been tested for previous versions or other platforms.
Download or clone and place the main folder into OF myApps. It should work anywhere as long as the OF_PATH is ../../.. 
If oyu have problems, the easier way is creating a new project with ‘project generator’ and adding the following addons.
Then substitute the files contained in src folder.

###### Addons
 * ofxMyo
 * ofxOsc
 * ofxXmlSettings
 * ofxBullet
 * ofxDatGui

###### troubleshooting
There might be problems with the libraries (Myo SDK) in which case I suggest to manually readdress them in the Build tab as indicated here
https://developer.thalmic.com/docs/api_reference/platform/the-sdk.html#library-structure


#### STRUCTURE

 The code counts different classes
 * Pulse Class - based on the soundbuffer example from OF, it controls the sounds
 * GUI Class - to control the GUI
 * myTheme - to control the theme of the GUI
 * WriteFile - to control the file saving


#### NOTES
 The program is working well and as expected. The code is prepared to have some interaction with 3d objects using bullet physics addon.
It is also prepared to load an external TXT file of a previous performance.

#### BUGS
Bugs: I haven't noticed any major bug at today.
 Some comment in the code report problems with tassellation using the ofPath to generate shapes. This problem has comments in the code.


#### CONTACTS
feel free to add, remove, edit, improve, destroy, munch, scratch the code as much as you like it.
Please let me know if you need help, or if you find alternatives or improvements to the code.
Also, if you reuse this code, it would be great to know what you have done!

http://www.letitbrain.it/LIB_interactive



