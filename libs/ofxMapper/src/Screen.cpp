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
			slice->draw();
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

const ofFbo & Screen::getFbo() const {
	return fbo;
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
void Screen::deselectSliceWarpers() {
    for (SlicePtr slice : slices) {
        slice->selected = false;
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
    mask->setScreenRect(getScreenRect());
    masks.push_back(mask);
    return mask;
}

//--------------------------------------------------------------
void Screen::deselectMasks() { 
    for (MaskPtr mask : masks) {
        mask->selected = false;
    }
}

//--------------------------------------------------------------
bool Screen::grabSlice(const glm::vec2 & p, float radius) {
	bool selected = false;
	for (SlicePtr slice : slices) {
		Warper * warper = slice->getWarper();
		if (slice->editEnabled && warper->grabHandle(p, radius)) {
			slice->selected = true;
			return true;
		}
	}
	for (SlicePtr slice : slices) {
		if (slice->selectWarper(p)) {
			selected = true;
		}
	}
	return selected;
}

//--------------------------------------------------------------
bool Screen::grabMask(const glm::vec2 & p, float radius) {
	bool selected = false;
	for (MaskPtr mask : masks) {
		if (mask->editEnabled && mask->grabHandle(p, radius)) {
			mask->selected = true;
			return true;
		}
	}
	for (MaskPtr mask : masks) {
		if (mask->select(p)) {
			selected = true;
		}
	}
	return selected;
}

//--------------------------------------------------------------
void Screen::grab(const glm::vec2 & p, float radius) {

	if (grabMask(p, radius)) {
		deselectSliceWarpers();
		return;
	}
	grabSlice(p, radius);
}

//--------------------------------------------------------------
void Screen::drag(const glm::vec2 & delta) {
	bool drag = false;
	for (MaskPtr mask : masks) {
		if (mask->editEnabled && mask->dragHandle(delta)) {
			drag = true;
		}
	}
	if (drag)
		return;

	for (SlicePtr slice : slices) {
		Warper * warper = slice->getWarper();
		if (slice->editEnabled && warper->dragHandle(delta)) {
			drag = true;
		}
	}
}

//--------------------------------------------------------------
void Screen::release() {
	for (MaskPtr mask : masks) {
		mask->releaseHandle();
	}

	for (SlicePtr slice : slices) {
		Warper * warper = slice->getWarper();
		warper->releaseHandle();
	}
}

//--------------------------------------------------------------
void Screen::resolutionChanged(int &) {
	fbo.allocate(width, height, GL_RGB);
	fbo.begin();
	ofClear(ofColor::black);
	fbo.end();

    ofRectangle screenRect = getScreenRect();
    for (MaskPtr mask : masks) {
        mask->setScreenRect(screenRect);
    }
}
