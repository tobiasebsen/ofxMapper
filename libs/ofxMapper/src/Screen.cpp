#include "Screen.h"

using namespace ofxMapper;

//--------------------------------------------------------------
Screen::Screen(int w, int h) {
	uniqueId = ofToString((uint64_t)ofRandom(9999999999999));

    width.addListener(this, &Screen::resolutionChanged);
	height.addListener(this, &Screen::resolutionChanged);

    width.setWithoutEventNotifications(w);
    height.set(h);
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

//--------------------------------------------------------------
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
	return addSlice(0, 0, width, height);
}

//--------------------------------------------------------------
SlicePtr Screen::addSlice(float x, float y, float width, float height) {
    return addSlice("Slice " + ofToString(slices.size()+1), ofRectangle(x, y, width, height));
}

//--------------------------------------------------------------
SlicePtr Screen::addSlice(string name, const ofRectangle & inputRect, const ofRectangle & outputRect) {
	slices.emplace_back(new Slice(inputRect.x, inputRect.y, inputRect.width, inputRect.height));
    SlicePtr slice = slices.back();
	slice->name = name;
	slice->createVertices(outputRect);
	return slice;
}

//--------------------------------------------------------------
SlicePtr Screen::addSlice(string name, const ofRectangle & inputRect) {
	return addSlice(name, inputRect, getScreenRect());
}

//--------------------------------------------------------------
void Screen::removeSlice(size_t sliceIndex) {
	slices.erase(slices.begin() + sliceIndex);
}

//--------------------------------------------------------------
bool Screen::selectSliceInput(const glm::vec2 & p) {
	bool selection = false;
	for (SlicePtr slice : slices) {
		if (slice->selectInput(p))
			selection = true;
	}
	return selection;
}

//--------------------------------------------------------------
bool ofxMapper::Screen::grabInputHandle(const glm::vec2 & p, float radius) {
	bool selected = false;
	for (SlicePtr slice : slices) {
		if (slice->grabInputHandle(p, radius)) {
			slice->selected = true;
			return true;
		}
	}
	for (SlicePtr slice : slices) {
		if (slice->selectInput(p)) {
			selected = true;
		}
	}
	return selected;
}

//--------------------------------------------------------------
bool ofxMapper::Screen::dragInputHandle(const glm::vec2 & delta) {
	bool dragged = false;
	for (SlicePtr slice : slices) {
		if (slice->dragInputHandle(delta)) {
			dragged = true;
		}
	}
	return dragged;
}

//--------------------------------------------------------------
void ofxMapper::Screen::releaseInputHandle() {
	for (SlicePtr slice : slices) {
		slice->releaseHandle();
	}
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
void Screen::removeMaskSelected() {
	for (auto it = masks.begin(); it != masks.end(); ) {
		MaskPtr mask = *it;
		if (mask->selected && !mask->removeHandleSelected()) {
			it = masks.erase(it);
		}
		else
			++it;
	}
}

//--------------------------------------------------------------
void Screen::deselectMasks() { 
    for (MaskPtr mask : masks) {
        mask->selected = false;
    }
}

//--------------------------------------------------------------
bool Screen::grabSliceHandle(const glm::vec2 & p, float radius) {
	bool selected = false;
	for (SlicePtr slice : slices) {
		if (slice->editEnabled && slice->grabHandle(p, radius)) {
			slice->selected = true;
			return true;
		}
	}
	for (SlicePtr slice : slices) {
		if (slice->select(p)) {
			selected = true;
		}
	}
	return selected;
}

//--------------------------------------------------------------
bool ofxMapper::Screen::dragSliceHandle(const glm::vec2 & delta) {
	bool drag = false;
	for (auto & slice : slices) {
		if (slice->dragHandle(delta))
			drag = true;
	}
	return drag;
}

//--------------------------------------------------------------
void ofxMapper::Screen::releaseSliceHandle() {
	for (auto & slice : slices) {
		slice->releaseHandle();
	}
}

//--------------------------------------------------------------
bool Screen::grabMaskHandle(const glm::vec2 & p, float radius) {
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
bool ofxMapper::Screen::dragMaskHandle(const glm::vec2 & delta) {
	bool dragged = false;
	for (MaskPtr mask : masks) {
		if (mask->dragHandle(delta)) {
			dragged = true;
		}
	}
	return dragged;
}

//--------------------------------------------------------------
void ofxMapper::Screen::releaseMaskHandle() {
	for (MaskPtr mask : masks) {
		mask->releaseHandle();
	}
}

//--------------------------------------------------------------
void Screen::drawHandles(int handleSize, int handleType) {

	float radius = handleSize / 2;

	ofPushStyle();
	for (auto & slice : slices) {
		auto & handles = slice->getHandles();
		for (auto & h : handles) {
			
			if (h.selected)
				ofFill();
			else
				ofNoFill();

			if (handleType == HANDLE_SQUARE)
				ofDrawRectangle(h.position.x - radius, h.position.y - radius, handleSize, handleSize);
			if (handleType == HANDLE_CIRCLE)
				ofDrawCircle(h.position, handleSize);
		}
	}
	for (auto & mask : masks) {
		auto & handles = mask->getHandles();
		for (auto & h : handles) {

			if (h.selected)
				ofFill();
			else
				ofNoFill();

			if (handleType == HANDLE_SQUARE)
				ofDrawRectangle(h.position.x - radius, h.position.y - radius, handleSize, handleSize);
			if (handleType == HANDLE_CIRCLE)
				ofDrawCircle(h.position, handleSize);
		}
	}
	ofPopStyle();
}

//--------------------------------------------------------------
bool ofxMapper::Screen::grabHandles(const glm::vec2 & p, float radius) {
	return grabMaskHandle(p, radius) || grabSliceHandle(p, radius);
}

//--------------------------------------------------------------
bool ofxMapper::Screen::dragHandles(const glm::vec2 & delta) {
	return dragMaskHandle(delta) || dragSliceHandle(delta);
}

//--------------------------------------------------------------
void ofxMapper::Screen::releaseHandles() {
	releaseMaskHandle();
	releaseSliceHandle();
}

//--------------------------------------------------------------
void Screen::resolutionChanged(int &) {

	if (width <= 0 || height <= 0)
		return;

	fbo.allocate(width, height, GL_RGB);
	fbo.begin();
	ofClear(ofColor::black);
	fbo.end();

    ofRectangle screenRect = getScreenRect();
    for (MaskPtr mask : masks) {
        mask->setScreenRect(screenRect);
    }
}
