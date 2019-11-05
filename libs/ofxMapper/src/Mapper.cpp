#include "Mapper.h"
#include "ResolumeLoader.h"

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

ScreenPtr Mapper::getScreen(size_t screenIndex) {
	return screens[screenIndex];
}

ScreenPtr ofxMapper::Mapper::addScreen(int width, int height) {
	return addScreen("Screen " + ofToString(screens.size()+1), width, height);
}

//--------------------------------------------------------------
ScreenPtr Mapper::addScreen(string name, int width, int height, bool enabled) {
	size_t screenIndex = screens.size();
	shared_ptr<Screen> screen(new Screen);
	screen->name = name;
	screen->width.setWithoutEventNotifications(width);
	screen->height.set(height);
	screen->enabled = enabled;
	//screen->remove.addListener(this, &ofApp::screenRemovePressed);
	screens.push_back(screen);
	return screen;
}

//--------------------------------------------------------------
void Mapper::removeScreen() {
	for (auto it = screens.begin(); it != screens.end();) {
		auto p = it[0];
		if (p->remove) {
			//p->remove.removeListener(this, &ofApp::screenRemovePressed);
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
void Mapper::sliceInputSelect(glm::vec2 p) {
	for (auto & screen : screens) {
		if (screen->enabled) {
			screen->selectSliceInput(p);
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

						ofRectangle blendRect = inputRect1.getIntersection(inputRect2);
						if (!blendRect.isEmpty() && blendRect.getArea() < inputRect1.getArea()/2.f) {

							slice1->addBlendRect(blendRect);
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
bool Mapper::load(string filePath) {
	ofXml xml;
	bool loaded = xml.load(filePath);

	if (loaded) {

		ResolumeLoader resolume(xml);

		if (resolume.isValid()) {
			// Composition
			ofRectangle r = resolume.getCompositionSize();
			setCompSize(r.width, r.height);

			// Screens
			screens.clear();
			int nscreens = resolume.loadScreens();
			for (int i = 0; i < nscreens; i++) {
				ResolumeLoader::Screen sc = resolume.getScreen(i);
				string name = sc.getName();
				ofRectangle res = sc.getSize();
				bool enabled = sc.getEnabled();
				ScreenPtr screen = addScreen(name, res.width, res.height, enabled);

				// Slices
				int nslices = sc.loadSlices();
				for (int j = 0; j < nslices; j++) {
					ResolumeLoader::Slice sl = sc.getSlice(j);
					string name = sl.getName();
					ofRectangle inputRect = sl.getInputRect();
					SlicePtr slice = screen->addSlice(name, inputRect);
					slice->enabled = sl.getEnabled();
					slice->softEdgeEnabled = sl.getSoftEdgeEnabled();

					SoftEdgePtr softEdge = slice->getSoftEdge();
					softEdge->power = sl.getSoftEdgePower();
					softEdge->luminance = sl.getSoftEdgeLuminance();
					softEdge->gamma = sl.getSoftEdgeGamma();

					slice->setInputRect(inputRect);
					slice->bezierEnabled = sl.getWarperMode() == "PM_BEZIER";

					int w = 0, h = 0;
					sl.getWarperDim(w, h);
					auto vertices = sl.getWarperVertices();
					slice->setVertices(vertices, w, h);
				}

				// Masks
				int nmasks = sc.loadMasks();
				for (int j = 0; j < nmasks; j++) {
					ResolumeLoader::Mask msk = sc.getMask(j);
					string name = msk.getName();
					auto points = msk.getPoints();

					MaskPtr mask = screen->addMask(name);
					mask->setPoints(points);
				}
			}
			compFileName = ofFilePath::getFileName(filePath);
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
string Mapper::getFileName() const {
	return compFileName;
}