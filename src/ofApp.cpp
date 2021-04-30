#include "ofApp.h"
#include "ofImage.h"

//--------------------------------------------------------------
void ofApp::setup(){

	// Load default files
	shader.load(shaderDir);
	//music.load(musicDir);

	// Set initial time
	timeOffset = ofGetElapsedTimeMillis();

	// Setup GUI
	gui.setup();
	gui.setPosition(ofGetWidth()-210,10);
	setTrackLength();
	gui.add(currentResolution.setup("Resolution", ofToString(ofGetWidth())+"x"+ofToString(ofGetHeight())));
	gui.add(recordingIndicator.setup("Recording", "false"));
	gui.add(playTime.setup("Time %", 0.0f, 0.0f, 1.0f));
	gui.add(fftSmoothness.setup("FFT Smoothness", 0.95f, 0.9f, 0.9999f));
	playTime.addListener(this, &ofApp::playTimeChanged);

	// Set up recorder
	ofSetFrameRate(frameRate);

}

//--------------------------------------------------------------
void ofApp::exit(){
	if (isRecording) {
		saveRecording();
	}
}

//--------------------------------------------------------------
void ofApp::setTrackLength(){
	music.play();
	music.setPosition(0.9999f);
	trackLength = music.getPositionMS();
	music.setPosition(0);
	music.stop();
}

//--------------------------------------------------------------
void ofApp::playTimeChanged(float &playTime){
	if(!updatingGUI && !isRecording){
		timeOffset = ofGetElapsedTimeMillis()-int(playTime*trackLength);
		music.setPosition(playTime);
	}
}

//--------------------------------------------------------------
void ofApp::saveRecording() {
	ofSystemAlertDialog("Saving video...");
	ofSystem("ffmpeg -ts_from_file 2 -framerate " + ofToString(frameRate) + " -i data/" + tempDir + "/%06d" + imageExt + " -i \"" + musicDir + "\" -c:v libx264 -c:a aac -b:a 192k -shortest " + videoDir + "/" + ofGetTimestampString() + videoExt);
	ofSystem("rm data/" + tempDir + "/*");
}

//--------------------------------------------------------------
void ofApp::update() {

	// Update music
	ofSoundUpdate();
	if (!musicPaused) {
		updatingGUI = true;
		playTime = music.getPosition();
		updatingGUI = false;
	}
	if(!music.isPlaying() && isRecording){
		isRecording = false;
		saveRecording();
	}
	
	// Update FFT
	if (!musicPaused) {
		// Get latest FFT
		float *val = ofSoundGetSpectrum(nBandsToGet);
		for (int i = 0;i < nBandsToGet; i++) {
			fftRaw[i] = val[i];
		}
	
		// Simplify the FFT to bands
		int currentSample = 0;
		for (int i = 0; i < 8; i++) {
			float average = 0;
			int sampleCount = pow(2, i) * 2;
			if (sampleCount == 7) {
				sampleCount+=2;
			}
			for (int j = 0; j < sampleCount; j++) {
				average += min(1.0f,fftRaw[currentSample]) * (currentSample + 1);
				currentSample++;
			}
			average /= currentSample;
			fftSimplifiedRaw[i] = average;
			fftSimplifiedSmooth[i] = max(fftSimplifiedSmooth[i]*fftSmoothness, average);
		}
	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	// Draw shader
	ofSetColor(255);
	shader.begin();
	shader.setUniform1f("time", float((musicPaused ? pausedTime : ofGetElapsedTimeMillis())-timeOffset)/1000);
	shader.setUniform1fv("fft", fftSimplifiedRaw, 8);
	shader.setUniform1fv("fftSmoothed", fftSimplifiedSmooth, 8);
	shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	shader.end();

	// Record the screen
	if (isRecording) {
		img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		img.save(tempDir + "/" + ofToString(framesRecorded, 6, 6, '0') + imageExt, imageQuality);
		framesRecorded++;
	}

	// Draw UI
	if(showUI){
		ofSetColor(0,0,0,150);
		ofDrawRectangle(10, 10, (showHelp ? 725 : 260), (showHelp ? 145 : 185));
		ofSetColor(255);
		ofDrawBitmapString(showHelp ? helpText : displayText,15,25);
		gui.draw();
		if(showFFT){
			//float width = (float)ofGetWidth()/8;
			float width = 20;
			ofSetColor(0,0,0,150);
			ofDrawRectangle(10, ofGetHeight()-10, 8*width+10, -125);
			ofSetColor(255);
			for (int i = 0;i < 8; i++){
				ofDrawRectangle(15+i*width,ofGetHeight()-30,width,-(fftSimplifiedSmooth[i]*25));
				ofDrawBitmapString(ofToString(i),15+i*width+width/4,ofGetHeight()-15);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch(key){

		case OF_KEY_F1:
			// Load music
			musicDir = ofSystemLoadDialog("Load music track").getPath();
			music.load(musicDir);
			setTrackLength();
			break;

		case OF_KEY_F2:
			// Set video output location
			videoDir = ofSystemLoadDialog("Select video export directory", true).getPath();
			break;

		case OF_KEY_F3:
			// Start recording
			if (isRecording) {
				saveRecording();
			}
			timeOffset = ofGetElapsedTimeMillis();
			music.play();
			music.setPaused(false);
			musicPaused = false;
			isRecording = true;
			recordingIndicator = "true";
			break;

		case OF_KEY_F4:
			// Stop recording
			music.setPaused(false);
			musicPaused = false;
			isRecording = false;
			music.stop();
			saveRecording();
			recordingIndicator = "false";
			break;

		case OF_KEY_F5:
			// Reload shader
			shader.load(shaderDir);
			break;

		case OF_KEY_F6:
			// Start music
			timeOffset = ofGetElapsedTimeMillis();
			music.play();
			music.setPaused(false);
			musicPaused = false;
			break;

		case OF_KEY_F7:
			// Pause music
			music.setPaused(!musicPaused);
			musicPaused = !musicPaused;
			if (musicPaused) {
				pausedTime = ofGetElapsedTimeMillis();
			} else {
				timeOffset = ofGetElapsedTimeMillis()-int(playTime*trackLength);
			}
			break;

		case OF_KEY_F8:
			// Stop music
			music.setPaused(false);
			musicPaused = false;
			music.stop();
			break;

		case OF_KEY_F9:
			ofToggleFullscreen();
			break;

		case OF_KEY_F10:
			// Toggle FFT
			showFFT = !showFFT;
			break;

		case OF_KEY_F11:
			// Toggle UI
			showUI = !showUI;
			break;

		case OF_KEY_F12:
			// Toggle help
			showHelp = !showHelp;
			break;

	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	gui.setPosition(w-210,10);
	currentResolution = ofToString(w) + "x" + ofToString(h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
