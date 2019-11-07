#include "BezierWarper.h"

ofParameter<int> BezierWarper::adaptiveBezierRes = {"Bezier span", 20, 10, 100};
ofParameter<int> BezierWarper::adaptiveSubdivRes = {"Subdivide span", 80, 10, 200};

//--------------------------------------------------------------
BezierWarper::BezierWarper() {
	cols = 0;
	rows = 0;
	adaptiveBezierRes.addListener(this, &BezierWarper::adaptiveBezierChanged);
	adaptiveSubdivRes.addListener(this, &BezierWarper::adaptiveSubdivChanged);
}

//--------------------------------------------------------------
BezierWarper::~BezierWarper() {
	adaptiveBezierRes.removeListener(this, &BezierWarper::adaptiveBezierChanged);
	adaptiveSubdivRes.removeListener(this, &BezierWarper::adaptiveSubdivChanged);
}

//--------------------------------------------------------------
void BezierWarper::setVertices(shared_ptr<glm::vec2> vertices, int controlWidth, int controlHeight) {

	this->controlWidth = controlWidth;
	this->controlHeight = controlHeight;
	this->vertices = vertices;

	rows = (controlHeight - 1) / 3;
	cols = (controlWidth - 1) / 3;

	update();
}

//--------------------------------------------------------------
void BezierWarper::update() {

    patches.resize(rows * cols);

	glm::vec2 * v = vertices.get();
    
    size_t rowIndex = 0;
    for (size_t r=0; r<rows; r++) {
        for (size_t c=0; c<cols; c++) {
            BezierPatch & patch = patches[rowIndex+c];

            int a, ac, bc, b;
            
			for (size_t i = 0; i < 4; i++) {
				a = r * controlWidth * 3 + controlWidth * i + c * 3;
				ac = a + 1;
				bc = ac + 1;
				b = bc + 1;
				patch.bezierRows[i].set(v[a], v[ac], v[bc], v[b], bezierResolution);
			}

			for (size_t i = 0; i < 4; i++) {
				a = r * controlWidth * 3 + c * 3 + i;
				ac = a + controlWidth;
				bc = ac + controlWidth;
				b = bc + controlWidth;
				patch.bezierCols[i].set(v[a], v[ac], v[bc], v[b], bezierResolution);
			}
        }
        rowIndex += cols;
    }

	makeOutline();
	makeSubdiv(); // -> makeMesh();
}

//--------------------------------------------------------------
void BezierWarper::setInputRect(shared_ptr<ofRectangle> inputRect) {
	this->inputRect = inputRect;
}

//--------------------------------------------------------------
void BezierWarper::makeSubdiv() {

    if (adaptive) {
        if (cols > 0 && rows > 0) {
            glm::vec2 v1 = patches[0].bezierRows[0].getStart();
            glm::vec2 v2 = patches[cols-1].bezierRows[0].getEnd();
            glm::vec2 v3 = patches[(rows-1)*cols].bezierRows[3].getStart();
            glm::vec2 v4 = patches[(rows-1)*cols+cols-1].bezierRows[3].getEnd();
            float d1 = glm::distance(v1, v2);
            float d2 = glm::distance(v3, v4);
            float d3 = glm::distance(v1, v3);
            float d4 = glm::distance(v2, v4);
			int sr = MAX(d1, d2) / adaptiveSubdivRes;
			int sc = MAX(d3, d4) / adaptiveSubdivRes;
			if (sr != subdivCols || sc != subdivRows) {
				subdivRows.setWithoutEventNotifications(sr);
				subdivCols.setWithoutEventNotifications(sc);
			}
        }
    }

    size_t rowIndex = 0;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            patches[rowIndex+c].subdivide(subdivCols, subdivRows);
        }
        rowIndex += cols;
    }
	makeMesh();
}

//--------------------------------------------------------------
void BezierWarper::makeOutline() {

	size_t rowIndex;

	outline.clear();

	// Top
	for (size_t c = 0; c < cols; c++) {
		auto & vts = patches[c].bezierRows[0].getVertices();
		for (auto & v : vts)
			outline.addVertex(ofVec3f(v));
	}
	// Right
	rowIndex = cols - 1;
	for (size_t r = 0; r < rows; r++) {
		auto & vts = patches[rowIndex].bezierCols[3].getVertices();
		for (auto & v : vts)
			outline.addVertex(ofVec3f(v));
		rowIndex += cols;
	}
	// Bottom
	rowIndex = (rows - 1) * cols;
	for (int c = cols-1; c >= 0; c--) {
		auto vts = patches[rowIndex + c].bezierRows[3].getVertices();
		std::reverse(vts.begin(), vts.end());
		for (auto & v : vts)
			outline.addVertex(ofVec3f(v));
	}
	// Left
	rowIndex = (rows - 1) * cols;
	for (int r = rows-1; r >= 0; r--) {
		auto vts = patches[rowIndex].bezierCols[0].getVertices();
		std::reverse(vts.begin(), vts.end());
		for (auto & v : vts)
			outline.addVertex(ofVec3f(v));
		rowIndex -= cols;
	}
	outline.close();
}

//--------------------------------------------------------------
void BezierWarper::makeMesh() {

	int n = mesh.getNumVertices();
    mesh.clearVertices();
    mesh.clearIndices();
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);

	vector<glm::vec3> vertices;
	vector<unsigned int> indices;

	unsigned int offsetIndex = 0;

	for (size_t r = 0; r < rows; r++) {
		for (size_t c = 0; c < cols; c++) {

			BezierPatch & patch = patches[r * cols + c];
			
			patch.meshVertices(vertices);
			mesh.addVertices(vertices);

			offsetIndex += patch.meshIndices(indices, offsetIndex);
			mesh.addIndices(indices);
		}
	}
	if (n != mesh.getNumVertices()) {
		updateTexCoords();
	}
}

//--------------------------------------------------------------
void BezierWarper::updateTexCoords() {

	mesh.clearTexCoords();

	vector<glm::vec2> texCoords;

	glm::vec2 offset(inputRect->x, inputRect->y);
	glm::vec2 delta;
	delta.s = inputRect->width / cols;
	delta.t = inputRect->height / rows;

	for (size_t r = 0; r < rows; r++) {
		for (size_t c = 0; c < cols; c++) {
			glm::vec2 uv0 = offset + delta * glm::vec2(c, r);
			glm::vec2 uv1 = uv0 + delta;

			BezierPatch & patch = patches[r * cols + c];
			patch.meshTexCoords(texCoords, uv0, uv1);
			mesh.addTexCoords(texCoords);

		}
	}
}

//--------------------------------------------------------------
void BezierWarper::drawGrid() {
    size_t rowIndex = 0;
    for (size_t r=0; r<rows; r++) {
        for (size_t c=0; c<cols; c++) {
            BezierPatch & patch = patches[rowIndex+c];
            drawPatch(patch);
        }
        rowIndex += cols;
    }
}

//--------------------------------------------------------------
void BezierWarper::drawSubGrid() {
    size_t rowIndex = 0;
    for (size_t r=0; r<rows; r++) {
        for (size_t c=0; c<cols; c++) {
            drawBeziers(patches[rowIndex+c].bezierSubRows);
            drawBeziers(patches[rowIndex+c].bezierSubCols);
        }
        rowIndex += cols;
    }
}

//--------------------------------------------------------------
void BezierWarper::drawOutline() {
	outline.draw();
}

//--------------------------------------------------------------
void BezierWarper::drawMesh() {
	mesh.draw();
}

//--------------------------------------------------------------
void BezierWarper::drawMesh(SoftEdgePtr softEdge) {
	softEdge->begin();
	softEdge->setUniforms(inputRect.get());
	mesh.draw();
	softEdge->end();
}

//--------------------------------------------------------------
void BezierWarper::drawPatch(BezierPatch & patch) {
    glEnableClientState(GL_VERTEX_ARRAY);
    drawBezier(patch.bezierRows[0]);
    drawBezier(patch.bezierRows[3]);
    drawBezier(patch.bezierCols[0]);
    drawBezier(patch.bezierCols[3]);
    glDisableClientState(GL_VERTEX_ARRAY);
}

//--------------------------------------------------------------
void BezierWarper::drawBezier(Bezier & bezier) {
    vector<glm::vec2> & vertices = bezier.getVertices();
    glVertexPointer(2, GL_FLOAT, 0, vertices.data());
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
}

//--------------------------------------------------------------
void BezierWarper::drawBeziers(vector<Bezier> & beziers) {
    glEnableClientState(GL_VERTEX_ARRAY);
    for (Bezier & b : beziers) {
        vector<glm::vec2> & vertices = b.getVertices();
        glVertexPointer(2, GL_FLOAT, 0, vertices.data());
        glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}

//--------------------------------------------------------------
bool BezierWarper::select(const glm::vec2 & p) {
	return outline.size() && outline.inside(ofPoint(p));
}

//--------------------------------------------------------------
void BezierWarper::moveHandle(WarpHandle & handle, const glm::vec2 & delta) {
	glm::vec2 * v = vertices.get();
	handle.position = (v[handle.vertexIndex] += delta);
	for (auto & h : handles) {
		moveHandle(h, delta);
	}
	notifyHandles();
}

//--------------------------------------------------------------
void BezierWarper::clearHandles() {
	handles.clear();
}

//--------------------------------------------------------------
void BezierWarper::addHandle(WarpHandle * parent, int x, int y) {
	ControlHandle h;
	h.parent = parent;
	h.vertexIndex = parent->vertexIndex + controlWidth * y + x;
	h.position = vertices.get()[h.vertexIndex];
	handles.push_back(h);
}

//--------------------------------------------------------------
void BezierWarper::updateHandles(vector<WarpHandle>& warpHandles) {
	handles.clear();

	for (auto & handle : warpHandles) {
		if (handle.selected) {

			if (handle.col < cols) {
				addHandle(&handle, 1, 0);
				if (handle.row < rows)
					addHandle(&handle, 1, 1);
				if (handle.row > 0)
					addHandle(&handle, 1, -1);
			}
			if (handle.col > 0) {
				addHandle(&handle, -1, 0);
				if (handle.row < rows)
					addHandle(&handle, -1, 1);
				if (handle.row > 0)
					addHandle(&handle, -1, -1);
			}
			if (handle.row < rows) {
				addHandle(&handle, 0, 1);
				/*if (handle.col < cols)
					addHandle(&handle, 1, 1);
				if (handle.col > 0)
					addHandle(&handle, -1, 1);*/
			}
			if (handle.row > 0) {
				addHandle(&handle, 0, -1);
				/*if (handle.col < cols)
					addHandle(&handle, 1, -1);*/
				/*if (handle.col > 0)
					addHandle(&handle, -1, -1);*/
			}
		}
	}
}

//--------------------------------------------------------------
void BezierWarper::moveHandle(ControlHandle & handle, const glm::vec2 & delta) {
	glm::vec2 * v = vertices.get();
	handle.position = (v[handle.vertexIndex] += delta);
}

void BezierWarper::notifyHandles() {
	update();
}

//--------------------------------------------------------------
/*WarpControls BezierWarper::getHandleControls(WarpHandle & handle) {
	WarpControls controls;
	glm::vec2 * v = vertices.get();

	if (handle.col > 0) {
		controls.left.vertexIndex = handle.vertexIndex - 1;
		controls.left.position = v[controls.left.vertexIndex];
	}
	return controls;
}*/

//--------------------------------------------------------------
/*PatchJunction BezierWarper::getHandlePatches(WarpHandle & handle) {
    PatchJunction junc;

    if (handle.col < cols) {
        if (handle.row < rows)
            junc.topLeft = &patches[handle.row * cols + handle.col];
        if (handle.row > 0)
            junc.bottomLeft = &patches[(handle.row - 1) * cols + handle.col];
    }
    if (handle.col > 0) {
        if (handle.row > 0)
            junc.bottomRight =  &patches[(handle.row - 1) * cols + handle.col - 1];
        if (handle.row < rows)
            junc.topRight = &patches[handle.row * cols + handle.col - 1];
    }
    return junc;
}*/
/*
//--------------------------------------------------------------
void BezierWarper::moveHandle(BezierHandle & handle, glm::vec2 & delta) {
    PatchJunction junction = getHandlePatches(handle);
    if (junction.topLeft)
        junction.topLeft->moveTopLeft(delta);
    if (junction.topRight)
        junction.topRight->moveTopRight(delta);
    if (junction.bottomLeft)
        junction.bottomLeft->moveBottomLeft(delta);
    if (junction.bottomRight)
        junction.bottomRight->moveBottomRight(delta);

    makeOutline();
    makeSubdiv();
	makeHandles();
}

//--------------------------------------------------------------
vector<glm::vec2> BezierWarper::getControls(BezierHandle & handle) {
    PatchJunction junction = getHandlePatches(handle);
	vector<glm::vec2> controls;
    if (junction.topLeft) {
        controls.push_back(junction.topLeft->bezierTop.getC1());
        controls.push_back(junction.topLeft->bezierLeft.getC1());
    }
    if (junction.topRight) {
        controls.push_back(junction.topRight->bezierTop.getC2());
        controls.push_back(junction.topRight->bezierRight.getC1());
    }
    if (junction.bottomLeft) {
        controls.push_back(junction.bottomLeft->bezierBottom.getC1());
        controls.push_back(junction.bottomLeft->bezierLeft.getC2());
    }
    if (junction.bottomRight) {
        controls.push_back(junction.bottomRight->bezierBottom.getC2());
        controls.push_back(junction.bottomRight->bezierRight.getC2());
    }
	return controls;
}*/

//--------------------------------------------------------------
void BezierWarper::adaptiveBezierChanged(int &) {
    if (adaptive) {
        for (BezierPatch & p : patches) {
            //p.setResolution(bezierResolution);
        }
    }
}

//--------------------------------------------------------------
void BezierWarper::adaptiveSubdivChanged(int &) {
    if (adaptive) {
        makeSubdiv();
    }
}
