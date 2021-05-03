#pragma once

#include "ofImage.h"
#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// UI
	const string displayText =  "STRKTURE v0.1\n"
								"[F1]: Load music\n"
								"[F2]: Set video output location\n"
								"[F3]: Start recording\n"
								"[F4]: Stop recording\n"
								"[F5]: Reload shader\n"
								"[F6]: Start music\n"
								"[F7]: Pause/Resume music\n"
								"[F8]: Stop music\n"
								"[F9]: Toggle fullscreen\n"
								"[F10]: Toggle FFT viewer\n"
								"[F11]: Toggle UI\n"
								"[F12]: Show help";
	const string helpText = "HOW TO USE:\n"
							"Select the audio track by pressing [F1]\n"
							"Select the video output location by pressing [F2]\n"
							"The default shaders are located at ./bin/data/shaders\n"
							"You can modify the default shaders and press [F5] to reload them without exiting Strkture\n"
							"Once you have selected the audio and shaders you can start recording by pressing [F3]\n"
							"Only the shader output is recorded so there is no need to hide the UI while recording\n"
							"The recording will automatically stop once the track finishes\n"
							"You can also stop recording prematurely by pressing [F4]\n"
							"Press [F12] to go back to the main menu\n";
	//bool fullscreen = false;
	bool showUI = true;
	bool showHelp = false;
	bool showFFT = true;
	
	// GUI
	ofxPanel gui;
	ofxFloatSlider playTime;
	ofxFloatSlider fftSmoothness;
	ofxLabel currentResolution;
	ofxLabel recordingIndicator;
	ofxLabel forceTimeIndicator;
	void playTimeChanged(float &playTime);
	void setTrackLength();
	bool updatingGUI = false;
	
	// Shader
	ofShader shader;
	string shaderDir = "shaders/shader";
	
	// Sound
	string musicDir = "music/s1.mp3";
	ofSoundPlayer music;
	int trackLength;
	bool musicPaused = false;
	int pausedTime;
	
	// FFT
	static constexpr int nBandsToGet = 512;
	std::array<float, nBandsToGet> fftRaw{{0}};
	std::array<float, nBandsToGet> fftSmoothed{{0}};
	float fftSimplifiedRaw[8];
	float fftSimplifiedSmooth[8];
	float fftInc[8] = {0};
	
	// Recorder
	ofImage img;
	bool isRecording = false;
	int frameRate = 30;
	float recordingFrequency = (1.0f/frameRate)*1000;
	const string tempDir = ".temp";
	string videoDir = "data/videos";
	string imageExt = ".jpg";
	string videoExt = ".mp4";
	ofImageQualityType imageQuality = OF_IMAGE_QUALITY_BEST;
	int framesRecorded;
	void saveRecording();

	// Others
	int timeOffset;

};
