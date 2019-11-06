#pragma once

#include "ofMain.h"
#include "Slice.h"
#include "Mask.h"

class Screen {
public:

	Screen();
	~Screen();

	ofRectangle getScreenRect();

	void update(ofTexture & inputTexture);

	void draw();
	void draw(const ofRectangle & rect);

	const ofFbo & getFbo() const;

    // Slices
    vector<SlicePtr> & getSlices();
	size_t getNumSlices() const;
	SlicePtr getSlice(size_t sliceIndex);
	SlicePtr addSlice(float width, float height);
	SlicePtr addSlice(string name, const ofRectangle & inputRect);
	SlicePtr addSlice(string name, const ofRectangle & inputRect, const ofRectangle & outputRect);
	void removeSlice(size_t sliceIndex);
    bool selectSliceInput(const glm::vec2 & p);

    void deselectSliceWarpers();
	void deselectMasks();

    // Masks
    vector<MaskPtr> & getMasks();
    size_t getNumMasks() const;
    MaskPtr getMask(size_t maskIndex);
    MaskPtr addMask(string name);
    void removeMask(size_t maskIndex);


	bool grabSlice(const glm::vec2 & p, float radius);
	bool grabMask(const glm::vec2 & p, float radius);

	void grab(const glm::vec2 & p, float radius);
	void drag(const glm::vec2 & delta);
	void release();

	ofParameter<string> name = { "Name:", "" };
	ofParameter<int> width = { "Width", 1920, 320, 3840 };
	ofParameter<int> height = { "Height", 1920, 320, 3840 };
	ofParameter<float> keystoneH = { "Keystone H", 0, -10, 10 };
	ofParameter<float> keystoneV = { "Keystone V", 0, -10, 10 };
	ofParameter<bool> enabled = { "Enabled", true };
	ofParameter<bool> remove = { "Remove", false };
	ofParameterGroup group = { "Screen" , name, width, height, enabled, remove };

	void resolutionChanged(int &);

private:
	ofFbo fbo;
	vector<SlicePtr> slices;
    vector<MaskPtr> masks;

	vector<ElementPtr> selectedElements;
};

typedef shared_ptr<Screen> ScreenPtr;
