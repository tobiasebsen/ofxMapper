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

    // Slices
    vector<SlicePtr> & getSlices();
	size_t getNumSlices() const;
	SlicePtr getSlice(size_t sliceIndex);
	SlicePtr addSlice(float width, float height);
	SlicePtr addSlice(string name, const ofRectangle & inputRect);
	SlicePtr addSlice(string name, const ofRectangle & inputRect, const ofRectangle & outputRect);
	void removeSlice(size_t sliceIndex);
    bool selectSliceInput(const glm::vec2 & p);
    bool selectSliceWarper(const glm::vec2 & p);
    void deselectSliceWarpers();
    
    bool grabHandle(const glm::vec2 & p, float radius);
	bool grabHandle(float x, float y, float radius);
	void dragHandle(const glm::vec2 & delta);
	void dragHandle(float dx, float dy);
    void releaseHandle();

    // Masks
    vector<MaskPtr> & getMasks();
    size_t getNumMasks() const;
    MaskPtr getMask(size_t maskIndex);
    MaskPtr addMask(string name);
    void removeMask(size_t maskIndex);
    bool selectMask(const glm::vec2 & p);
    void deselectMasks();

	ofParameter<string> name = { "Name:", "" };
	ofParameter<int> width = { "Width", 1920, 320, 3840 };
	ofParameter<int> height = { "Height", 1920, 320, 3840 };
	ofParameter<bool> enabled = { "Enabled", true };
	ofParameter<bool> remove = { "Remove", false };
	ofParameterGroup group = { "Screen" , name, width, height, enabled, remove };
	ofFbo fbo;

	void resolutionChanged(int &);

private:
	vector<SlicePtr> slices;
    vector<MaskPtr> masks;
};

typedef shared_ptr<Screen> ScreenPtr;
