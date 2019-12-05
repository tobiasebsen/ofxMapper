#include "Mapper.h"

using namespace ofxMapper;

//--------------------------------------------------------------
void Mapper::setup() {
	setup(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void Mapper::setup(int width, int height) {
	setCompSize(width, height);
}

//--------------------------------------------------------------
void Mapper::begin() {
	fbo.begin();
}

//--------------------------------------------------------------
void Mapper::end() {
	fbo.end();

	update(fbo.getTexture());
}

//--------------------------------------------------------------
void Mapper::update(ofTexture & texture) {
    
    updateBlendRects();

	for (auto & screen : screens) {
		screen->update(texture);
	}
}

//--------------------------------------------------------------
void Mapper::draw() {
	ofPushMatrix();
	for (auto & screen : screens) {
		if (screen->enabled) {
			screen->draw();
			ofTranslate(screen->width, 0);
		}
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void Mapper::drawComp() {
	fbo.draw(0, 0);
}

//--------------------------------------------------------------
void Mapper::drawInputRects(bool drawDisabled, bool drawDeselected) {
	for (auto & screen : screens) {
		size_t n = screen->getNumSlices();
		for (size_t i = 0; i < n; i++) {
			SlicePtr slice = screen->getSlice(i);
			if ((slice->enabled || drawDisabled) && (!slice->selected || drawDeselected)) {
				slice->drawInputRect();
			}
		}
	}
}

//--------------------------------------------------------------
void Mapper::drawInputRectsSelected(bool drawDisabled) {
	for (auto & screen : screens) {
		size_t n = screen->getNumSlices();
		for (size_t i = 0; i < n; i++) {
			SlicePtr slice = screen->getSlice(i);
			if ((slice->enabled || drawDisabled) && slice->selected) {
				slice->drawInputRect();
			}
		}
	}
}

//--------------------------------------------------------------
void Mapper::setCompSize(size_t width, size_t height) {
	fbo.allocate(width, height, GL_RGB);
	compRect.set(0, 0, width, height);
}

//--------------------------------------------------------------
ofFbo & Mapper::getFbo() {
	return fbo;
}

//--------------------------------------------------------------
ofRectangle Mapper::getCompRect() const {
	return compRect;
}

//--------------------------------------------------------------
vector<ScreenPtr> & Mapper::getScreens() {
    return screens;
}

//--------------------------------------------------------------
size_t Mapper::getNumScreens() const {
	return screens.size();
}

//--------------------------------------------------------------
ScreenPtr Mapper::getScreen(size_t screenIndex) {
	return screens[screenIndex];
}

//--------------------------------------------------------------
ScreenPtr ofxMapper::Mapper::addScreen(int width, int height) {
	return addScreen("Screen " + ofToString(screens.size()+1), width, height);
}

//--------------------------------------------------------------
ScreenPtr Mapper::addScreen(string name, int width, int height, bool enabled) {
    screens.emplace_back(new Screen(width, height));
    ScreenPtr screen = screens.back();
	screen->name = name;
	screen->enabled = enabled;
	return screen;
}

//--------------------------------------------------------------
void Mapper::removeScreen() {
	for (auto it = screens.begin(); it != screens.end();) {
		auto p = it[0];
		if (p->remove) {
			it = screens.erase(it);
		}
		else {
			++it;
		}
	}
}

//--------------------------------------------------------------
void Mapper::clearScreens() {
	screens.clear();
}

//--------------------------------------------------------------
void Mapper::deselectAll() {
    for (ScreenPtr screen : screens) {
        screen->deselectSliceWarpers();
        screen->deselectMasks();
    }
}

//--------------------------------------------------------------
void Mapper::deselectAllExcept(ScreenPtr except) {
    for (ScreenPtr screen : screens) {
        if (screen != except) {
            screen->deselectSliceWarpers();
            screen->deselectMasks();
        }
    }
}

//--------------------------------------------------------------
void Mapper::grabInputHandle(const glm::vec2 & p, float radius) {
	for (auto & screen : screens) {
		if (screen->enabled) {
			if (screen->grabInputHandle(p, radius))
				return;
		}
	}
	for (auto & screen : screens) {
		if (screen->enabled) {
			screen->selectSliceInput(p);
		}
	}
}

//--------------------------------------------------------------
void ofxMapper::Mapper::dragInputHandle(const glm::vec2 & delta) {
	for (auto & screen : screens) {
		if (screen->enabled) {
			screen->dragInputHandle(delta);
		}
	}
}

//--------------------------------------------------------------
void ofxMapper::Mapper::releaseInputHandle() {
	for (auto & screen : screens) {
		if (screen->enabled) {
			screen->releaseInputHandle();
		}
	}
}

//--------------------------------------------------------------
void Mapper::updateBlendRects() {
    size_t n = getNumScreens();
    for (size_t i = 0; i < n; i++) {
        ScreenPtr screen1 = getScreen(i);

        size_t m = screen1->getNumSlices();
        for (size_t j = 0; j < m; j++) {
            SlicePtr slice1 = screen1->getSlice(j);
			ofRectangle inputRect1 = slice1->getInputRect();
            slice1->clearBlendRects();
            
            for (size_t k=0; k<n; k++) {
                if (k != i) {
                    ScreenPtr screen2 = getScreen(k);

                    size_t h = screen2->getNumSlices();
                    for (size_t g=0; g<h; g++) {
                        SlicePtr slice2 = screen2->getSlice(g);
						ofRectangle inputRect2 = slice2->getInputRect();

						if (slice1->softEdgeEnabled && slice2->softEdgeEnabled) {
							ofRectangle blendRect = inputRect1.getIntersection(inputRect2);
							if (!blendRect.isEmpty() && blendRect.getArea() < inputRect1.getArea() / 2.f) {
								slice1->addBlendRect(blendRect);
							}
						}
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void Mapper::drawBlendRects() {
    for (auto & screen : screens) {
        size_t n = screen->getNumSlices();
        for (size_t i = 0; i < n; i++) {
            SlicePtr slice = screen->getSlice(i);
            auto & blendRects = slice->getBlendRects();
            for (auto & rect : blendRects) {
                ofDrawRectangle(rect);
            }
        }
    }
}

//--------------------------------------------------------------
void ofxMapper::Mapper::clear() {
	compFile.reset();
	screens.clear();
}

//--------------------------------------------------------------
bool Mapper::load(string filePath) {

	compFile = shared_ptr<ResolumeFile>(new ResolumeFile);
	bool loaded = compFile->load(filePath);

	if (!loaded) {
		ofLogError("ofxMapper") << "Unable to load file: " << filePath;
		compFile.reset();
		return false;
	}

	if (!compFile->isValid("Resolume Arena") && !compFile->isValid("ofxMapper")) {
		ofLogError("ofxMapper") << "File not valid: " << filePath;
		compFile.reset();
		return false;
	}

	// Composition
	ofRectangle r = compFile->getCompositionSize();
	setCompSize(r.width, r.height);

	// Screens
	screens.clear();
	int nscreens = compFile->loadScreens();
	for (int i = 0; i < nscreens; i++) {
		ResolumeFile::Screen sc = compFile->getScreen(i);
		ofRectangle res = sc.getSize();
		ScreenPtr screen = addScreen(res.width, res.height);
		screen->uniqueId = sc.getUniqueId();
		screen->name = sc.getName();
		screen->enabled = sc.getEnabled();

		// Slices
		int nslices = sc.getNumSlices();
		for (int j = 0; j < nslices; j++) {
			ResolumeFile::Slice sl = sc.getSlice(j);
			string name = sl.getName();
			ofRectangle inputRect = sl.getInputRect();
			SlicePtr slice = screen->addSlice(name, inputRect);
			slice->uniqueId = sl.getUniqueId();
			slice->enabled = sl.getEnabled();
			slice->softEdgeEnabled = sl.getSoftEdgeEnabled();

			SoftEdge & softEdge = slice->getSoftEdge();
			softEdge.power = sl.getSoftEdgePower(softEdge.power);
			softEdge.luminance = sl.getSoftEdgeLuminance(softEdge.luminance);
			softEdge.gamma = sl.getSoftEdgeGamma(softEdge.gamma);

			slice->setInputRect(inputRect);
			slice->bezierEnabled = sl.getWarperMode() == "PM_BEZIER";

			int w = 0, h = 0;
			sl.getWarperDim(w, h);
			auto vertices = sl.getWarperVertices();
			slice->setVertices(vertices, w, h);
		}

		// Masks
		int nmasks = sc.getNumMasks();
		for (int j = 0; j < nmasks; j++) {
			ResolumeFile::Mask msk = sc.getMask(j);
			string name = msk.getName();
			auto points = msk.getPoints();

			MaskPtr mask = screen->addMask(name);
			mask->uniqueId = msk.getUniqueId();
			mask->enabled = msk.getEnabled();
			mask->closed = msk.getClosed();
			mask->setPoints(points);
		}
	}
	compFilePath = filePath;

	return true;
}

//--------------------------------------------------------------
void ofxMapper::Mapper::save(string filePath) {

	//ResolumeFile saver(xml);
	if (!compFile) {
		compFile = shared_ptr<ResolumeFile>(new ResolumeFile);
		compFile->setVersion("ofxMapper");
	}

	compFile->setCompositionSize(compRect.width, compRect.height);

	for (auto & screen : screens) {
		ResolumeFile::Screen scr = compFile->getScreen(screen->uniqueId);
		scr.setName(screen->name);
		scr.setEnabled(screen->enabled);

		for (auto & slice : screen->getSlices()) {
			ResolumeFile::Slice slc = scr.getSlice(slice->uniqueId);
			slc.setName(slice->name);
			slc.setEnabled(slice->enabled);
			slc.setInputRect(slice->getInputRect());
			slc.setWarperMode(slice->bezierEnabled ? "PM_BEZIER" : "PM_LINEAR");

			slc.setWarperVertices(slice->getControlWidth(), slice->getControlHeight(), slice->getVertices());
		}

		for (auto & mask : screen->getMasks()) {
			ResolumeFile::Mask msk = scr.getMask(mask->uniqueId);
			msk.setName(mask->name);
			msk.setEnabled(mask->enabled);
			msk.setInverted(mask->inverted);

			vector<glm::vec2> points;
			for (auto & h : mask->getHandles()) {
				points.push_back(h.position);
			}
			msk.setPoints(points, mask->closed);
		}

		scr.setSize(screen->width, screen->height);
	}

	if (compFile->save(filePath))
		compFilePath = filePath;
}

//--------------------------------------------------------------
void ofxMapper::Mapper::save() {
	save(compFilePath);
}

//--------------------------------------------------------------
string Mapper::getFileName() const {
	return ofFilePath::getFileName(compFilePath);
}

string ofxMapper::Mapper::getFilePath() const {
	return compFilePath;
}
