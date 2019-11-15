#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	grabber.setup(320, 240);

	// Add a virtual screen of specified size
	screen = mapper.addScreen(ofGetWidth(), ofGetHeight());

	// Add a slice to the screen
	slice = screen->addSlice(grabber.getWidth(), grabber.getHeight());
	slice->editEnabled = true;
}

//--------------------------------------------------------------
void ofApp::update(){

	grabber.update();

	// Update (draw) warped slice to screen fbo
	mapper.update(grabber.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw(){

	// Draw screen to window
	mapper.draw();

	// Draw handles
	screen->drawHandles();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == OF_KEY_RIGHT)
		slice->moveHandle(1.f, 0.f);
	if (key == OF_KEY_DOWN)
		slice->moveHandle(0.f, 1.f);
	if (key == OF_KEY_LEFT)
		slice->moveHandle(-1.f, 0.f);
	if (key == OF_KEY_UP)
		slice->moveHandle(0.f, -1.f);
	if (key == 'x')
		slice->subdivide(1, 0);
	if (key == 'y')
		slice->subdivide(0, 1);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	slice->dragHandle(x - ofGetPreviousMouseX(), y - ofGetPreviousMouseY());
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	slice->grabHandle(x, y, 10);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	slice->releaseHandle();
}
