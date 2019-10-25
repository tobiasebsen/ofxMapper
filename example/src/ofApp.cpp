#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	grabber.setup(320, 240);

	auto screen = mapper.addScreen(ofGetWidth(), ofGetHeight());
	screen->addSlice(grabber.getWidth(), grabber.getHeight());
}

//--------------------------------------------------------------
void ofApp::update(){

	grabber.update();

	mapper.update(grabber.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw(){
	mapper.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	mapper.getScreen()->dragHandle(x - ofGetPreviousMouseX(), y - ofGetPreviousMouseY());
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	mapper.getScreen()->grabHandle(x, y, 10);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	mapper.getScreen()->releaseHandle();
}
