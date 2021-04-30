#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3,2);
	settings.setSize(1024, 720);
	settings.setGLVersion(4, 4);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}
