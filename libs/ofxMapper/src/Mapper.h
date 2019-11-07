#pragma once

#include "ofMain.h"
#include "Screen.h"

namespace ofxMapper {

	class Mapper {
	public:

		// Setup internal frame buffer
		void setup();
		void setup(int width, int height);

		// Begin/end drawing on frame buffer
		void begin();
		void end();

		// Update and draw mapped content
		void update(ofTexture & texture);

		// Draw mapped content
		void draw();


		////////////////////////////////////////////////////////////


		// File

		bool load(string filePath);
		void save(string filePath);
		void save();
		string getFileName() const;


		// Composition

		void drawComp();
		void setCompSize(size_t width, size_t height);
		ofRectangle getCompRect() const;
		ofFbo & getFbo();


		// Screens

		vector<ScreenPtr> & getScreens();
		size_t getNumScreens() const;
		ScreenPtr getScreen(size_t screenIndex = 0);
		ScreenPtr addScreen(int width, int height);
		ScreenPtr addScreen(string name, int width, int height, bool enabled = true);
		void removeScreen();
		void clearScreens();
		void deselectAll();
		void deselectAllExcept(ScreenPtr screen);


		// Slices

		void drawInputRects(bool drawDisabled = false, bool drawDeselected = true);
		void drawInputRectsSelected(bool drawDisabled = false);
		void sliceInputSelect(glm::vec2 p);
		void updateBlendRects();
		void drawBlendRects();

	private:
		ofRectangle compRect;

		string compFileName = "untitled.xml";

		// Frame buffer
		ofFbo fbo;

		vector<ScreenPtr> screens;
	};

}