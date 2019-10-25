#include "Slice.h"


//--------------------------------------------------------------
Slice::Slice() {
	warper = &linearWarper;

	inputRect = shared_ptr<ofRectangle>(new ofRectangle);
	bezierWarper.setInputRect(inputRect);
    linearWarper.setInputRect(inputRect);

	controlWidth = 0;
	controlHeight = 0;
	gridCols = 0;
	gridRows = 0;

	inputX.addListener(this, &Slice::inputRectChanged);
	inputY.addListener(this, &Slice::inputRectChanged);
	inputWidth.addListener(this, &Slice::inputRectChanged);
	inputHeight.addListener(this, &Slice::inputRectChanged);
    
    bezierEnabled.addListener(this, &Slice::bezierChanged);

	softEdge = shared_ptr<SoftEdge>(new SoftEdge);
}

//--------------------------------------------------------------
Slice::~Slice() {
	inputX.removeListener(this, &Slice::inputRectChanged);
	inputY.removeListener(this, &Slice::inputRectChanged);
	inputWidth.removeListener(this, &Slice::inputRectChanged);
	inputHeight.removeListener(this, &Slice::inputRectChanged);
}

//--------------------------------------------------------------
void Slice::setInputRect(ofRectangle inputRect) {
	this->inputRect->x = inputRect.x;
	this->inputRect->y = inputRect.y;
	this->inputRect->width = inputRect.width;
	this->inputRect->height = inputRect.height;
	inputX.set(inputRect.x); // trigger event
}

//--------------------------------------------------------------
ofRectangle Slice::getInputRect() {
	return ofRectangle(inputRect->x, inputRect->y, inputRect->width, inputRect->height);
}

void Slice::setVertices(vector<glm::vec2>& vertices, size_t controlWidth, size_t controlHeight) {
	this->controlWidth = controlWidth;
	this->controlHeight = controlHeight;

	size_t n = vertices.size();
	this->vertices = shared_ptr<glm::vec2>(new glm::vec2[n]);
	std::copy(vertices.begin(), vertices.end(), this->vertices.get());

	linearWarper.setVertices(this->vertices, controlWidth, controlHeight);
    bezierWarper.setVertices(this->vertices, controlWidth, controlHeight);

	updateHandles();
}

void Slice::createVertices(const ofRectangle & rect) {

	this->controlWidth = 4;
	this->controlHeight = 4;

	size_t n = controlWidth * controlHeight;;
	vertices = shared_ptr<glm::vec2>(new glm::vec2[n]);
	glm::vec2 * v = vertices.get();

	glm::vec2 v0 = rect.getTopLeft();
	glm::vec2 v1 = rect.getBottomRight();

	for (size_t r = 0; r < controlHeight; r++) {
		for (size_t c = 0; c < controlWidth; c++) {
			glm::vec2 delta = glm::vec2(c / 3.f, r / 3.f);
			v[r * controlWidth + c] = glm::mix(v0, v1, delta);
		}
	}

	linearWarper.setVertices(this->vertices, controlWidth, controlHeight);
	bezierWarper.setVertices(this->vertices, controlWidth, controlHeight);

	updateHandles();
}

void Slice::update() {
	warper->update();
}

//--------------------------------------------------------------
void Slice::drawInputRect() {
	ofPushStyle();
	ofNoFill();
	ofDrawRectangle(getInputRect());
	ofPopStyle();
}

//--------------------------------------------------------------
void Slice::drawGrid() {
	warper->drawGrid();
}

//--------------------------------------------------------------
void Slice::drawSubGrid() {
	warper->drawSubGrid();
}

//--------------------------------------------------------------
void Slice::drawMesh() {
	if (softEdgeEnabled)
		warper->drawMesh(softEdge);
	else
		warper->drawMesh();
}

//--------------------------------------------------------------
bool Slice::selectInput(const glm::vec2 & p) {
	selected = getInputRect().inside(ofPoint(p));
	return selected;
}

//--------------------------------------------------------------
bool Slice::selectWarper(const glm::vec2 & p) {
	selected = warper->select(p);
    return selected;
}

//--------------------------------------------------------------
void Slice::updateHandles() {
	gridCols = (controlWidth - 1) / 3 + 1;
	gridRows = (controlHeight - 1) / 3 + 1;

	handles.resize(gridCols * gridRows);

	glm::vec2 * v = vertices.get();
	
	size_t rowIndex = 0;
	for (size_t r = 0; r < gridRows; r++) {
		for (size_t c = 0; c < gridCols; c++) {
			WarpHandle & handle = handles[rowIndex + c];

			handle.vertexIndex = r * controlWidth * 3 + c * 3;
			handle.position = v[handle.vertexIndex];

			/*handle.position = v + (r * controlWidth * 3) + (c * 3);

			if (c < gridCols-1)
				handle.controlRight.position = handle.position + 1;
			if (r < gridRows-1)
				handle.controlDown.position = handle.position + controlWidth;

			if (c > 0)
				handle.controlLeft.position = handle.position - 1;
			if (r > 0)
				handle.controlUp.position = handle.position - controlWidth;*/

			handle.gridCol = c;
			handle.gridRow = r;
			handle.selected = false;
			handle.dragging = false;
		}
		rowIndex += gridCols;
	}
}

//--------------------------------------------------------------
vector<WarpHandle> & Slice::getHandles() {
	return handles;
}

//--------------------------------------------------------------
bool Slice::selectHandle(const glm::vec2 & p, float radius) {
    bool selected = false;
    for (WarpHandle & h : handles) {
        if (glm::distance(p, h.position) < radius)
            selected = true;
    }
    return selected;
}

//--------------------------------------------------------------
bool Slice::grabHandle(const glm::vec2 & p, float radius) {
    bool grabbed = false;
    for (WarpHandle & h : handles) {
        if (glm::distance(p, h.position) < radius) {
			grabbed = true;
            h.selected = true;
            h.dragging = true;
        }
		else {
			h.selected = false;
			h.dragging = false;
		}
    }
    return grabbed;
}

//--------------------------------------------------------------
void Slice::dragHandle(const glm::vec2 & delta) {
    for (WarpHandle & h : handles) {
        if (h.dragging) {
            //h.movePosition(delta);
			warper->moveVertex(h.gridCol, h.gridRow, delta);
			h.position += delta;
        }
    }
}

//--------------------------------------------------------------
void Slice::releaseHandle() {
    for (WarpHandle & h : handles) {
        if (h.dragging) {
            h.dragging = false;
        }
    }
}

//--------------------------------------------------------------
void Slice::clearBlendRects() {
	softEdge->edgeTop = 0;
	softEdge->edgeBottom = 0;
	softEdge->edgeLeft = 0;
	softEdge->edgeRight = 0;
    blendRects.clear();
}

//--------------------------------------------------------------
bool Slice::addBlendRect(const ofRectangle & rect) {
    if (rect.getLeft() == inputRect->getLeft())
        softEdge->edgeLeft = rect.width / inputRect->width;
    if (rect.getRight() == inputRect->getRight())
        softEdge->edgeRight = rect.width / inputRect->width;

    blendRects.push_back(rect);
    return true;
}

//--------------------------------------------------------------
vector<ofRectangle> & Slice::getBlendRects() {
    return blendRects;
}

//--------------------------------------------------------------
SoftEdgePtr Slice::getSoftEdge() {
	return softEdge;
}

//--------------------------------------------------------------
void Slice::inputRectChanged(int &) {
	inputRect->x = inputX;
	inputRect->y = inputY;
	inputRect->width = inputWidth;
	inputRect->height = inputHeight;
	warper->updateTexCoords();
}

//--------------------------------------------------------------
void Slice::bezierChanged(bool &) {
    if (bezierEnabled)
        warper = &bezierWarper;
    else
        warper = &linearWarper;

    warper->update();
    warper->updateTexCoords();
}
