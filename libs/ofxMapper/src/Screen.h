#pragma once

#include "ofMain.h"
#include "Slice.h"
#include "Mask.h"

namespace ofxMapper {

	class Screen {
	public:
		~Screen();

		// Frame buffer

		ofRectangle getScreenRect();
		ofRectangle getScreenSize();
		glm::vec2 getScreenPos();
		void update(ofTexture & inputTexture);
		const ofFbo & getFbo() const;

		void draw();
		void draw(const ofRectangle & rect);

		// Slices
		vector<SlicePtr> & getSlices();
		size_t getNumSlices() const;
		SlicePtr getSlice(size_t sliceIndex);
		SlicePtr addSlice(float width, float height);
        SlicePtr addSlice(float x, float y, float width, float height);
		SlicePtr addSlice(string name, const ofRectangle & inputRect);
		SlicePtr addSlice(string name, const ofRectangle & inputRect, const ofRectangle & outputRect);
		void removeSlice(size_t sliceIndex);
		
		bool selectSliceInput(const glm::vec2 & p);
		bool grabInputHandle(const glm::vec2 & p, float radius);
		void dragInputHandle(const glm::vec2 & delta);
		void moveInputHandle(const glm::vec2 & delta);
		void releaseInputHandle();
		
		void deselectSliceWarpers();
		bool grabSlice(const glm::vec2 & p, float radius);
		void moveSlice(const glm::vec2 & delta);
		void dragSlice(const glm::vec2 & delta);
		void releaseSlice();

		// Masks
		vector<MaskPtr> & getMasks();
		size_t getNumMasks() const;
		MaskPtr getMask(size_t maskIndex);
		MaskPtr addMask(string name);
		void removeMaskSelected();
		void deselectMasks();
		bool grabMask(const glm::vec2 & p, float radius);
		void moveMask(const glm::vec2 & delta);
		void dragMask(const glm::vec2 & delta);
		void releaseMask();

		// Handles
		enum { HANDLE_SQUARE, HANDLE_CIRCLE };
		void drawHandles(int handleSize = 10, int handleType = HANDLE_SQUARE);

		// Interaction
		bool grab(const glm::vec2 & p, float radius);
		void drag(const glm::vec2 & delta);
		void release();

		// Parameters
		string uniqueId;
		ofParameter<string> name = { "Name:", "" };
		ofParameter<int> posX = { "X", 0, 0, 7680 };
		ofParameter<int> posY = { "Y", 0, 0, 7680 };
		ofParameter<int> width = { "Width", 1920, 640, 3840 };
		ofParameter<int> height = { "Height", 1080, 480, 3840 };
		ofParameter<float> keystoneH = { "Keystone H", 0, -10, 10 };
		ofParameter<float> keystoneV = { "Keystone V", 0, -10, 10 };
		ofParameter<bool> enabled = { "Enabled", true };
		ofParameter<bool> remove = { "Remove", false };
		ofParameterGroup group = { "Screen" , name, posX, posY, width, height, enabled, remove };

	private:

        friend class Mapper;
        Screen(int x, int y, int width, int height);
		Screen(int width, int height);

        void resolutionChanged(int &);

		ofFbo fbo;
		vector<SlicePtr> slices;
		vector<MaskPtr> masks;

		vector<ElementPtr> selectedElements;
	};

	typedef shared_ptr<Screen> ScreenPtr;

}
