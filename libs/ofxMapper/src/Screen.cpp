#include "Screen.h"

using namespace ofxMapper;

//--------------------------------------------------------------
Screen::Screen(int w, int h) {
	Screen(0, 0, w, h);
}

//--------------------------------------------------------------
Screen::Screen(int x, int y, int w, int h) {
	uniqueId = ofToString((uint64_t)ofRandom(9999999999999));

	posX = x;
	posY = y;

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
	return ofRectangle(posX, posY, width, height);
}

//--------------------------------------------------------------
glm::vec2 ofxMapper::Screen::getScreenPos() {
	return glm::vec2(posX, posY);
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
	return selected;
}

//--------------------------------------------------------------
void ofxMapper::Screen::dragInputHandle(const glm::vec2 & delta) {
	for (SlicePtr slice : slices) {
		slice->dragInputHandle(delta);
	}
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
bool Screen::grabSlice(const glm::vec2 & p, float radius) {
	bool selected = false;
	for (SlicePtr slice : slices) {
		if (slice->editEnabled && slice->grabHandle(p, radius)) {
			slice->selected = true;
			return true;
		}
	}
	for (SlicePtr slice : slices) {
		if (slice->editEnabled) {
			if (slice->grab(p))
				selected = true;
		}
		else if (slice->select(p)) {
			selected = true;
		}
	}
	return selected;
}

//--------------------------------------------------------------
void ofxMapper::Screen::moveSlice(const glm::vec2 & delta) {
	for (SlicePtr slice : slices) {
		if (slice->selected && slice->editEnabled) {
			if (!slice->moveHandle(delta))
				slice->move(delta);
		}
	}
}

//--------------------------------------------------------------
void ofxMapper::Screen::dragSlice(const glm::vec2 & delta) {
	for (auto & slice : slices) {
		slice->dragHandle(delta);
		slice->drag(delta);
	}
}

//--------------------------------------------------------------
void ofxMapper::Screen::releaseSlice() {
	for (auto & slice : slices) {
		slice->releaseHandle();
		slice->release();
	}
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
		if (mask->editEnabled) {
			if (mask->grab(p))
				selected = true;
		}
		else if (mask->select(p)) {
			selected = true;
		}
	}
	return selected;
}

//--------------------------------------------------------------
void ofxMapper::Screen::moveMask(const glm::vec2 & delta) {
	for (MaskPtr mask : masks) {
		if (mask->selected && mask->editEnabled) {
			if (!mask->moveHandle(delta))
				mask->move(delta);
		}
	}
}

//--------------------------------------------------------------
void ofxMapper::Screen::dragMask(const glm::vec2 & delta) {
	for (MaskPtr mask : masks) {
		mask->dragHandle(delta);
		mask->drag(delta);
	}
}

//--------------------------------------------------------------
void ofxMapper::Screen::releaseMask() {
	for (MaskPtr mask : masks) {
		mask->releaseHandle();
		mask->release();
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
bool ofxMapper::Screen::grab(const glm::vec2 & p, float radius) {
	return grabMask(p, radius) || grabSlice(p, radius);
}

//--------------------------------------------------------------
void ofxMapper::Screen::drag(const glm::vec2 & delta) {
	dragMask(delta);
	dragSlice(delta);
}

//--------------------------------------------------------------
void ofxMapper::Screen::release() {
	releaseMask();
	releaseSlice();
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
