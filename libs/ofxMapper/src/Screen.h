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
		bool dragInputHandle(const glm::vec2 & delta);
		void releaseInputHandle();

		void deselectSliceWarpers();
		void deselectMasks();
		bool grabSliceHandle(const glm::vec2 & p, float radius);
		bool dragSliceHandle(const glm::vec2 & delta);
		void releaseSliceHandle();

		// Masks
		vector<MaskPtr> & getMasks();
		size_t getNumMasks() const;
		MaskPtr getMask(size_t maskIndex);
		MaskPtr addMask(string name);
		void removeMaskSelected();
		bool grabMaskHandle(const glm::vec2 & p, float radius);
		bool dragMaskHandle(const glm::vec2 & delta);
		void releaseMaskHandle();

		// Handles
		enum { HANDLE_SQUARE, HANDLE_CIRCLE };
		void drawHandles(int handleSize = 10, int handleType = HANDLE_SQUARE);
		bool grabHandles(const glm::vec2 & p, float radius);
		bool dragHandles(const glm::vec2 & delta);
		void releaseHandles();

		// Parameters
		string uniqueId;
		ofParameter<string> name = { "Name:", "" };
		ofParameter<int> width = { "Width", 1920, 320, 3840 };
		ofParameter<int> height = { "Height", 1920, 320, 3840 };
		ofParameter<float> keystoneH = { "Keystone H", 0, -10, 10 };
		ofParameter<float> keystoneV = { "Keystone V", 0, -10, 10 };
		ofParameter<bool> enabled = { "Enabled", true };
		ofParameter<bool> remove = { "Remove", false };
		ofParameterGroup group = { "Screen" , name, width, height, enabled, remove };

	private:

        friend class Mapper;
        Screen(int width, int height);

        void resolutionChanged(int &);

		ofFbo fbo;
		vector<SlicePtr> slices;
		vector<MaskPtr> masks;

		vector<ElementPtr> selectedElements;
	};

	typedef shared_ptr<Screen> ScreenPtr;

}
