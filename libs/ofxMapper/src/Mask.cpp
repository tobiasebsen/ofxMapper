#include "Mask.h"

using namespace ofxMapper;

ofTessellator tessellator;

Mask::Mask() {
	uniqueId = ofToString((uint64_t)ofRandom(9999999999999));
    closed.addListener(this, &Mask::closedChanged);
    inverted.addListener(this, &Mask::invertedChanged);
    poly.resize(2);
}

void Mask::setPoints(vector<glm::vec2> &points) {
	handles.clear();
    for (glm::vec2 & p : points) {
		DragHandle h;
		h.position = p;
		handles.push_back(h);
    }
	update();
}

void ofxMapper::Mask::addPoint(const glm::vec2 & p) {
	DragHandle h;
	h.position = p;
	handles.push_back(h);
	poly[0].addVertex(glm::vec3(h.position, 0));
}

void ofxMapper::Mask::setPoint(const glm::vec2 & p, int index) {
	if (index < 0)
		index = handles.size() - 1;
	handles[index].position = p;
	poly[0].getVertices().back() = glm::vec3(p, 0);
}

void ofxMapper::Mask::removePoint(int index) {
	if (index < 0)
		index = handles.size() - 1;
	if (index >= 0 && index < handles.size()) {
		handles.erase(handles.begin() + index);
		update();
	}
}

void Mask::setScreenRect(const ofRectangle &rect) {
    this->screenRect = rect;
}

void Mask::update() {
	poly[0].clear();
	for (DragHandle & h : handles) {
		poly[0].addVertex(glm::vec3(h.position, 0));
	}
    if (closed)
        poly[0].close();
    updateMesh();
}

void Mask::updateMesh() {
    if (closed) {
        tessellator.tessellateToMesh(poly, OF_POLY_WINDING_ODD, mesh);
    }
    else
        mesh.clear();
}

void Mask::draw() {
    if (closed)
        mesh.draw();
    else
        poly[0].draw();
}

void Mask::drawOutline() {
    poly[0].draw();
}

glm::vec2 Mask::getCenter() {
	return poly[0].getCentroid2D();
}

bool Mask::select(const glm::vec2 &p) {
    selected = poly[0].inside(p.x, p.y);
    return selected;
}

void Mask::move(const glm::vec2 & delta) {
	for (DragHandle & h : handles) {
		h.position += delta;
	}
	update();
}

bool Mask::removeHandleSelected() {
	bool removed = false;
	for (auto it = handles.begin(); it!=handles.end(); ) {
		DragHandle & handle = *it;
		if (handle.selected) {
			it = handles.erase(it);
			removed = true;
		}
		else
			++it;
	}
	if (removed)
		update();
	return removed;
}

void Mask::moveHandle(DragHandle & handle, const glm::vec2 & delta) {
	handle.position += delta;
}

void Mask::notifyHandles() {
	update();
}

void Mask::closedChanged(bool &) {
    poly[0].setClosed(closed);
    updateMesh();
}

void Mask::invertedChanged(bool &) {
    poly[1].clear();
    if (inverted) {
        poly[1].addVertex(screenRect.getTopLeft());
        poly[1].addVertex(screenRect.getTopRight());
        poly[1].addVertex(screenRect.getBottomRight());
        poly[1].addVertex(screenRect.getBottomLeft());
    }
    updateMesh();
}
