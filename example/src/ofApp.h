#pragma once

#include "ofMain.h"
#include "ofxMapper.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		
		ofVideoGrabber grabber;
		ofxMapper::Mapper mapper;
		ofxMapper::ScreenPtr screen;
		ofxMapper::SlicePtr slice;
};
