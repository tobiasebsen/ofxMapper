#include "Slice.h"

using namespace ofxMapper;

//--------------------------------------------------------------
Slice::Slice(float x, float y, float width, float height) {
	uniqueId = ofToString((uint64_t)ofRandom(9999999999999));

	warper = &linearWarper;

	inputX.addListener(this, &Slice::inputRectChanged);
	inputY.addListener(this, &Slice::inputRectChanged);
	inputWidth.addListener(this, &Slice::inputRectChanged);
	inputHeight.addListener(this, &Slice::inputRectChanged);
    
    bezierEnabled.addListener(this, &Slice::bezierChanged);

	ofRectangle inputRect(x, y, width, height);
	setInputRect(inputRect);
	bezierWarper.setInputRect(inputRect);
	linearWarper.setInputRect(inputRect);
}

//--------------------------------------------------------------
Slice::~Slice() {
	inputX.removeListener(this, &Slice::inputRectChanged);
	inputY.removeListener(this, &Slice::inputRectChanged);
	inputWidth.removeListener(this, &Slice::inputRectChanged);
	inputHeight.removeListener(this, &Slice::inputRectChanged);
}

//--------------------------------------------------------------
void Slice::setInputRect(const ofRectangle & inputRect) {
    inputX.setWithoutEventNotifications(inputRect.x);
    inputY.setWithoutEventNotifications(inputRect.y);
    inputWidth.setWithoutEventNotifications(inputRect.width);
    inputHeight.set(inputRect.height); // trigger event
}

//--------------------------------------------------------------
ofRectangle Slice::getInputRect() {
	return ofRectangle(inputX, inputY, inputWidth, inputHeight);
}

//--------------------------------------------------------------
void Slice::setVertices(vector<glm::vec2>& v, size_t controlWidth, size_t controlHeight) {

    vertices = shared_ptr<Vertices>(new Vertices(controlWidth, controlHeight));
	std::copy(v.begin(), v.end(), vertices->data);

	linearWarper.setVertices(vertices);
    bezierWarper.setVertices(vertices);

	updateHandles();
}

//--------------------------------------------------------------
void Slice::createVertices(const ofRectangle & rect) {

	/*this->controlWidth = 4;
	this->controlHeight = 4;

	size_t n = controlWidth * controlHeight;;
	vertices = shared_ptr<glm::vec2>(new glm::vec2[n]);
	glm::vec2 * v = vertices.get();*/
    vertices = shared_ptr<Vertices>(new Vertices(4, 4));
    glm::vec2 * v = vertices->data;

	glm::vec2 v0 = rect.getTopLeft();
	glm::vec2 v1 = rect.getBottomRight();

	for (size_t y = 0; y < vertices->height; y++) {
		for (size_t x = 0; x < vertices->width; x++) {
			glm::vec2 delta = glm::vec2(x / 3.f, y / 3.f);
			v[y * vertices->width + x] = glm::mix(v0, v1, delta);
		}
	}

	linearWarper.setVertices(vertices);
	bezierWarper.setVertices(vertices);

	//linearWarper.updateHandles();
	//bezierWarper.updateHandles();
	updateHandles();
}

//--------------------------------------------------------------
glm::vec2 * Slice::getVertices() {
	return vertices->data;
}

//--------------------------------------------------------------
size_t Slice::getControlWidth() {
	return vertices->width;
}

//--------------------------------------------------------------
size_t Slice::getControlHeight() {
	return vertices->height;
}

//--------------------------------------------------------------
void Slice::subdivide(int cols, int rows) {
    if (bezierEnabled)
        vertices = bezierWarper.subdivide(cols, rows);
    else
        vertices = linearWarper.subdivide(cols, rows);

    linearWarper.setVertices(vertices);
    bezierWarper.setVertices(vertices);

    updateHandles();
}

//--------------------------------------------------------------
void Slice::update() {
	warper->updatePatches();
}

//--------------------------------------------------------------
void Slice::drawInputRect() {
	ofPushStyle();
	ofNoFill();
	ofDrawRectangle(getInputRect());
	ofPopStyle();
}

//--------------------------------------------------------------
void ofxMapper::Slice::updateInputHandles() {
	inputHandles.resize(4);
	inputHandles[0].side = RectHandle::SIDE_TOP;
	inputHandles[0].position = glm::vec2(inputX + inputWidth / 2, inputY);
	inputHandles[1].side = RectHandle::SIDE_RIGHT;
	inputHandles[1].position = glm::vec2(inputX + inputWidth, inputY + inputHeight / 2);
	inputHandles[2].side = RectHandle::SIDE_BOTTOM;
	inputHandles[2].position = glm::vec2(inputX + inputWidth / 2, inputY + inputHeight);
	inputHandles[3].side = RectHandle::SIDE_LEFT;
	inputHandles[3].position = glm::vec2(inputX, inputY + inputHeight / 2);
}

//--------------------------------------------------------------
vector<RectHandle> & Slice::getInputHandles() {
	return inputHandles;
}

//--------------------------------------------------------------
bool ofxMapper::Slice::grabInputHandle(const glm::vec2 & p, float radius) {
	bool grabbed = false;
	for (auto & h : inputHandles) {
		if (glm::distance(p, h.position) <= radius) {
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
void ofxMapper::Slice::dragInputHandle(const glm::vec2 & delta) {
	for (auto & h : inputHandles) {
		if (h.dragging) {
			moveInputHandle(h, delta);
		}
	}
}

//--------------------------------------------------------------
void ofxMapper::Slice::moveInputHandle(RectHandle & handle, const glm::vec2 & delta) {
	switch (handle.side) {
	case RectHandle::SIDE_LEFT:
		inputX.setWithoutEventNotifications(handle.position.x + delta.x);
		inputWidth -= delta.x;
		break;
	case RectHandle::SIDE_TOP:
		inputY.setWithoutEventNotifications(handle.position.y + delta.y);
		inputHeight -= delta.y;
		break;
	case RectHandle::SIDE_RIGHT:
		inputWidth += delta.x;
		break;
	case RectHandle::SIDE_BOTTOM:
		inputHeight += delta.y;
		break;
	}
}

//--------------------------------------------------------------
void ofxMapper::Slice::releaseInputHandle() {
	for (auto & h : inputHandles) {
		h.dragging = false;
	}
}

//--------------------------------------------------------------
glm::vec2 Slice::getCenter() {
	return getInputRect().getCenter();
}

//--------------------------------------------------------------
void Slice::draw() {
    
    const ofShader & shader = warper->getShader();

    shader.begin();
    softEdge.setUniforms(shader, getInputRect());
    if (colorEnabled)
        colorCorrect.setUniforms(shader);
    else
        colorCorrect.setUniformsZero(shader);
    shader.end();

    warper->drawMesh();
}

//--------------------------------------------------------------
void Slice::drawOutline() {
	warper->drawOutline();
}

//--------------------------------------------------------------
bool Slice::selectInput(const glm::vec2 & p) {
	selected = getInputRect().inside(ofPoint(p));
	return selected;
}

//--------------------------------------------------------------
bool Slice::select(const glm::vec2 & p) {
	selected = warper->select(p);
    return selected;
}

//--------------------------------------------------------------
void ofxMapper::Slice::move(const glm::vec2 & delta) {
	size_t n = vertices->width * vertices->height;
	glm::vec2 * v = vertices->data;
	for (int i = 0; i < n; i++) {
		v[i] += delta;
	}
	update();
	updateHandles();
}

//--------------------------------------------------------------
Warper * Slice::getWarper() {
	return warper;
}

//--------------------------------------------------------------
BezierWarper & Slice::getBezierWarper() {
	return bezierWarper;
}

//--------------------------------------------------------------
void Slice::updateHandles() {

	if (!vertices)
		return;

	int gridCols = (vertices->width - 1) / 3 + 1;
	int gridRows = (vertices->height - 1) / 3 + 1;

	handles.resize(gridCols * gridRows);

	glm::vec2 * v = vertices->data;
	
	size_t rowIndex = 0;
	for (size_t r = 0; r < gridRows; r++) {
		for (size_t c = 0; c < gridCols; c++) {
			WarpHandle & handle = handles[rowIndex + c];

			handle.vertexIndex = r * vertices->width * 3 + c * 3;
			handle.position = v[handle.vertexIndex];

			handle.col = c;
			handle.row = r;
			handle.selected = false;
			handle.dragging = false;
		}
		rowIndex += gridCols;
	}
}

//--------------------------------------------------------------
bool Slice::grabHandle(const glm::vec2 & p, float radius) {
	bool grabbed = HasHandlesT<WarpHandle>::grabHandle(p, radius);
	if (bezierEnabled) {
		if (grabbed) {
			bezierWarper.updateHandles(handles);
		}
		else {
			grabbed = bezierWarper.grabHandle(p, radius);
			if (!grabbed)
				bezierWarper.clearHandles();
		}
	}
	return grabbed;
}

//--------------------------------------------------------------
void Slice::dragHandle(const glm::vec2 & delta) {
	HasHandlesT<WarpHandle>::dragHandle(delta);
	if (bezierEnabled) {
		bezierWarper.dragHandle(delta);
	}
}

//--------------------------------------------------------------
bool Slice::moveHandle(const glm::vec2 & delta) {
	bool moved = HasHandlesT<WarpHandle>::moveHandle(delta);
	if (bezierEnabled) {
		if (bezierWarper.moveHandle(delta))
			moved = true;
	}
	return moved;
}

//--------------------------------------------------------------
void Slice::moveHandle(WarpHandle & handle, const glm::vec2 & delta) {
	warper->moveHandle(handle, delta);
}

//--------------------------------------------------------------
void Slice::notifyHandles() {
	update();
}

//--------------------------------------------------------------
void Slice::clearBlendRects() {
	softEdge.edgeTop = 0;
	softEdge.edgeBottom = 0;
	softEdge.edgeLeft = 0;
	softEdge.edgeRight = 0;
    blendRects.clear();
}

//--------------------------------------------------------------
bool Slice::addBlendRect(const ofRectangle & rect) {
    if (rect.getLeft() == inputX)
        softEdge.edgeLeft = rect.width / inputWidth;
    if (rect.getRight() == inputX + inputWidth)
        softEdge.edgeRight = rect.width / inputWidth;

    blendRects.push_back(rect);
    return true;
}

//--------------------------------------------------------------
vector<ofRectangle> & Slice::getBlendRects() {
    return blendRects;
}

//--------------------------------------------------------------
SoftEdge & Slice::getSoftEdge() {
	return softEdge;
}

//--------------------------------------------------------------
ColorCorrect & Slice::getColorCorrect() {
    return colorCorrect;
}

//--------------------------------------------------------------
void Slice::inputRectChanged(int &) {
    ofRectangle inputRect = getInputRect();
    warper->setInputRect(inputRect);
	updateInputHandles();
}

//--------------------------------------------------------------
void Slice::bezierChanged(bool &) {
    if (bezierEnabled)
        warper = &bezierWarper;
    else
        warper = &linearWarper;

    warper->updatePatches();
    
    ofRectangle inputRect = getInputRect();
    warper->setInputRect(inputRect);
}
