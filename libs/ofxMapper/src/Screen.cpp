#include "Screen.h"

//--------------------------------------------------------------
Screen::Screen() {
	width.addListener(this, &Screen::resolutionChanged);
	height.addListener(this, &Screen::resolutionChanged);
	int dummy;
	resolutionChanged(dummy);
}

//--------------------------------------------------------------
Screen::~Screen() {
	width.removeListener(this, &Screen::resolutionChanged);
	height.removeListener(this, &Screen::resolutionChanged);
	fbo.clear();
}

//--------------------------------------------------------------
ofRectangle Screen::getScreenRect() {
	return ofRectangle(0, 0, width, height);
}

//--------------------------------------------------------------
void Screen::update(ofTexture & inputTexture) {

	fbo.begin();
	ofClear(0);

	inputTexture.bind();

	for (SlicePtr slice : slices) {
		if (slice->enabled) {
			slice->drawMesh();
		}
	}

	inputTexture.unbind();

	ofPushStyle();
	ofSetColor(ofColor::black);
	for (MaskPtr mask : masks) {
		if (mask->enabled) {
			mask->draw();
		}
	}
	ofPopStyle();

	fbo.end();
}

//--------------------------------------------------------------
void Screen::draw() {
	draw(getScreenRect());
}

//--------------------------------------------------------------
void Screen::draw(const ofRectangle & rect) {

	fbo.draw(rect);
}

//--------------------------------------------------------------
vector<SlicePtr> & Screen::getSlices() {
    return slices;
}

//--------------------------------------------------------------
size_t Screen::getNumSlices() const {
	return slices.size();
}

//--------------------------------------------------------------
SlicePtr Screen::getSlice(size_t sliceIndex) {
	return slices[sliceIndex];
}

//--------------------------------------------------------------
SlicePtr Screen::addSlice(float width, float height) {
	return addSlice("Slice " + ofToString(slices.size()+1), ofRectangle(0, 0, width, height));
}

//--------------------------------------------------------------
SlicePtr Screen::addSlice(string name, const ofRectangle & inputRect, const ofRectangle & outputRect) {
	SlicePtr slice(new Slice);
	slice->name = name;
	slice->inputX = inputRect.x;
	slice->inputY = inputRect.y;
	slice->inputWidth = inputRect.width;
	slice->inputHeight = inputRect.height;
	slices.push_back(slice);
	slices.back()->createVertices(outputRect);
	return slice;
}

SlicePtr Screen::addSlice(string name, const ofRectangle & inputRect) {
	return addSlice(name, inputRect, getScreenRect());
}

//--------------------------------------------------------------
void Screen::removeSlice(size_t sliceIndex) {
	slices.erase(slices.begin() + sliceIndex);
}

bool Screen::selectSliceInput(const glm::vec2 & p) {
	bool selection = false;
	for (SlicePtr slice : slices) {
		if (slice->selectInput(p))
			selection = true;
	}
	return selection;
}

//--------------------------------------------------------------
bool Screen::selectSliceWarper(const glm::vec2 &p) {
    bool selection = false;
    for (SlicePtr slice : slices) {
        if (slice->selectWarper(p)) {
            selection = true;
        }
    }
    return selection;
}

//--------------------------------------------------------------
void Screen::deselectSliceWarpers() {
    for (SlicePtr slice : slices) {
        slice->selected = false;
    }
}

//--------------------------------------------------------------
bool Screen::grabHandle(const glm::vec2 & p, float radius) {
	bool grabbed = false;
	for (SlicePtr slice : slices) {
		if (slice->grabHandle(p, radius))
			grabbed = true;
	}
	return grabbed;
}

//--------------------------------------------------------------
bool Screen::grabHandle(float x, float y, float radius) {
	return grabHandle(glm::vec2(x, y), radius);
}

//--------------------------------------------------------------
void Screen::dragHandle(const glm::vec2 & delta) {
	for (SlicePtr slice : slices) {
		slice->dragHandle(delta);
	}
}

//--------------------------------------------------------------
void Screen::dragHandle(float dx, float dy) {
	dragHandle(glm::vec2(dx, dy));
}

//--------------------------------------------------------------
void Screen::releaseHandle() {
	for (SlicePtr slice : slices) {
		slice->releaseHandle();
	}
}


//--------------------------------------------------------------
vector<MaskPtr> & Screen::getMasks() {
    return masks;
}

//--------------------------------------------------------------
size_t Screen::getNumMasks() const {
    return masks.size();
}

//--------------------------------------------------------------
MaskPtr Screen::getMask(size_t maskIndex) {
    return masks[maskIndex];
}

//--------------------------------------------------------------
MaskPtr Screen::addMask(string name) {
    MaskPtr mask(new Mask);
    mask->name = name;
    masks.push_back(mask);
    return mask;
}

//--------------------------------------------------------------
bool Screen::selectMask(const glm::vec2 &p) {
    bool selection = false;
    for (MaskPtr mask : masks) {
        if (mask->select(p)) {
            selection = true;
        }
    }
    return selection;
}

//--------------------------------------------------------------
void Screen::deselectMasks() { 
    for (MaskPtr mask : masks) {
        mask->selected = false;
    }
}

//--------------------------------------------------------------
void Screen::resolutionChanged(int &) {
	fbo.allocate(width, height, GL_RGB);
	fbo.begin();
	ofClear(ofColor::black);
	fbo.end();
}