#include "BezierWarper.h"
#include "ColorCorrect.h"

#define STR(a) #a

static string fragHeader = "#version 120\n"
STR(
    uniform sampler2DRect tex;
);

static string fragMain =
STR(
    void main() {
        vec2 texCoord = gl_TexCoord[0].st;
        vec2 uv = (texCoord - pos) / size;
        vec4 sample = texture2DRect(tex, texCoord);
        sample = colorCorrect(sample);
        sample = softEdge(sample, uv);
        
        gl_FragColor = sample;
    }
);


ofShader BezierWarper::shader;

ofParameter<int> BezierWarper::adaptiveBezierRes = {"Bezier span", 50, 10, 100};
ofParameter<int> BezierWarper::adaptiveSubRes = {"Sub-bezier span", 50, 10, 200};

//--------------------------------------------------------------
BezierWarper::BezierWarper() {
	cols = 0;
	rows = 0;
	adaptiveBezierRes.addListener(this, &BezierWarper::adaptiveBezierChanged);
	adaptiveSubRes.addListener(this, &BezierWarper::adaptiveSubChanged);
}

//--------------------------------------------------------------
BezierWarper::~BezierWarper() {
	adaptiveBezierRes.removeListener(this, &BezierWarper::adaptiveBezierChanged);
	adaptiveSubRes.removeListener(this, &BezierWarper::adaptiveSubChanged);
}

//--------------------------------------------------------------
void BezierWarper::setVertices(VerticesPtr v) {

	vertices = v;

    cols = (vertices->width - 1) / 3;
	rows = (vertices->height - 1) / 3;

	updatePatches();
}

//--------------------------------------------------------------
VerticesPtr BezierWarper::subdivide(int subdivCols, int subdivRows) {

    int subWidth = (vertices->width-1) * subdivCols + 1;
    int subHeight = (vertices->height-1) * subdivRows + 1;
    shared_ptr<Vertices> subvertices = shared_ptr<Vertices>(new Vertices(subWidth, subHeight));
    
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            BezierPatch & patch = patches[r * cols + c];

			int row = r * subdivRows * 3;
			int col = c * subdivCols * 3;
			
			vector<Bezier> subRowTop = patch.bezierRows[0].subdivide(subdivCols-1);
            vector<Bezier> subRowBot = patch.bezierRows[3].subdivide(subdivCols-1);

			for (int x = 0; x < subdivCols; x++) {

				glm::vec2 v0 = subRowTop[x].getStart();
				glm::vec2 v1 = subRowTop[x].getC1();
				glm::vec2 v2 = subRowTop[x].getC2();
				glm::vec2 v3 = subRowTop[x].getEnd();

				glm::vec2 w0 = subRowBot[x].getStart();
				glm::vec2 w1 = subRowBot[x].getC1();
				glm::vec2 w2 = subRowBot[x].getC2();
				glm::vec2 w3 = subRowBot[x].getEnd();

				for (int y=0; y<=subdivRows; y++) {
					float f = (float)y / (float)subdivRows;
                    
					int j = (row + y * 3) * subWidth + (col + x * 3);

					glm::vec2 u0 = mix(v0, w0, f);
					glm::vec2 u3 = mix(v3, w3, f);

					subvertices->data[j+0] = u0;
					subvertices->data[j+1] = u0 + mix(v1, w1, f);
					subvertices->data[j+2] = u3 + mix(v2, w2, f);
					subvertices->data[j+3] = u3;
                }
            }

			vector<Bezier> subColLeft = patch.bezierCols[0].subdivide(subdivRows - 1);
			vector<Bezier> subColRight = patch.bezierCols[3].subdivide(subdivRows - 1);

			for (int y = 0; y < subdivRows; y++) {

				glm::vec2 v0 = subColLeft[y].getStart();
				glm::vec2 v1 = subColLeft[y].getC1();
				glm::vec2 v2 = subColLeft[y].getC2();
				glm::vec2 v3 = subColLeft[y].getEnd();

				glm::vec2 w0 = subColRight[y].getStart();
				glm::vec2 w1 = subColRight[y].getC1();
				glm::vec2 w2 = subColRight[y].getC2();
				glm::vec2 w3 = subColRight[y].getEnd();

				for (int x = 0; x <= subdivCols; x++) {
					float f = (float)x / (float)subdivCols;

					int j = (row + y * 3) * subWidth + (col + x * 3);

					glm::vec2 u0 = mix(v0, w0, f);
					glm::vec2 u1 = mix(v1, w1, f);
					glm::vec2 u2 = mix(v2, w2, f);
					glm::vec2 u3 = mix(v3, w3, f);

					if (x == 0 || x == subdivCols) {
						subvertices->data[j + 0 * subWidth] = u0;
						subvertices->data[j + 1 * subWidth] = u0 + mix(v1, w1, f);
						subvertices->data[j + 2 * subWidth] = u3 + mix(v2, w2, f);
						subvertices->data[j + 3 * subWidth] = u3;
					}
					else {
						glm::vec2 t0 = subvertices->data[j + 0 * subWidth];
						subvertices->data[j + 1 * subWidth] = t0 + u1;
						glm::vec2 t3 = subvertices->data[j + 3 * subWidth];
						subvertices->data[j + 2 * subWidth] = t3 + u2;
					}
				}
			}

			for (int y = 0; y < subdivRows; y++) {
				for (int x = 0; x < subdivCols; x++) {
					int j = (row + y * 3) * subWidth + (col + x * 3);

					glm::vec2 a0 = subvertices->data[j + 0];
					glm::vec2 c00 = subvertices->data[j + 1] - a0;
					glm::vec2 c01 = subvertices->data[j + subWidth] - a0;
					subvertices->data[j + subWidth + 1] = a0 + c00 + c01;

					glm::vec2 b0 = subvertices->data[j + 3];
					glm::vec2 c10 = subvertices->data[j + 2] - b0;
					glm::vec2 c11 = subvertices->data[j + subWidth + 3] - b0;
					subvertices->data[j + subWidth + 2] = b0 + c10 + c11;

					j += subWidth * 3;

					glm::vec2 a1 = subvertices->data[j + 0];
					glm::vec2 d00 = subvertices->data[j + 1] - a1;
					glm::vec2 d01 = subvertices->data[j - subWidth] - a1;
					subvertices->data[j - subWidth + 1] = a1 + d00 + d01;

					glm::vec2 b1 = subvertices->data[j + 3];
					glm::vec2 d10 = subvertices->data[j + 2] - b1;
					glm::vec2 d11 = subvertices->data[j - subWidth + 3] - b1;
					subvertices->data[j - subWidth + 2] = b1 + d10 + d11;
				}
			}
		}
    }
    return subvertices;
}


//--------------------------------------------------------------
void BezierWarper::updatePatches() {

    patches.resize(rows * cols);

	glm::vec2 * v = vertices->data;

    size_t rowIndex = 0;
    for (size_t r=0; r<rows; r++) {
        for (size_t c=0; c<cols; c++) {
            BezierPatch & patch = patches[rowIndex+c];

            int a, ac, bc, b;
            
			for (size_t i = 0; i < 4; i++) {
				a = r * vertices->width * 3 + vertices->width * i + c * 3;
				ac = a + 1;
				bc = ac + 1;
				b = bc + 1;
				int res = adaptive ? (int)(Bezier::getApproxLength(v[a], v[ac], v[bc], v[b]) / adaptiveBezierRes) : (int)bezierResolution;
				patch.bezierRows[i].set(v[a], v[ac], v[bc], v[b], res);
			}

			for (size_t i = 0; i < 4; i++) {
				a = r * vertices->width * 3 + c * 3 + i;
				ac = a + vertices->width;
				bc = ac + vertices->width;
				b = bc + vertices->width;
				int res = adaptive ? (int)(Bezier::getApproxLength(v[a], v[ac], v[bc], v[b]) / adaptiveBezierRes) : (int)bezierResolution;
				patch.bezierCols[i].set(v[a], v[ac], v[bc], v[b], res);
			}
        }
        rowIndex += cols;
    }

	makeOutline();
	makeSub(); // -> makeMesh();
}

//--------------------------------------------------------------
void BezierWarper::setInputRect(ofRectangle & inputRect) {
	this->inputRect = inputRect;
    updateTexCoords();
}

//--------------------------------------------------------------
void BezierWarper::makeSub() {

    if (adaptive) {

		float colLength = 0;
		float rowLength = 0;

		for (BezierPatch & patch : patches) {

			for (int i = 0; i < 4; i++) {
				float length = patch.bezierCols[i].getLength();
				if (length > colLength)
					colLength = length;
			}
			for (int i = 0; i < 4; i++) {
				float length = patch.bezierRows[i].getLength();
				if (length > rowLength)
					rowLength = length;
			}
		}

		if (colLength > 0 && rowLength > 0) {
			int sc = colLength / adaptiveSubRes;
			int sr = rowLength / adaptiveSubRes;
			if (sr != subCols || sc != subRows) {
				subCols.setWithoutEventNotifications(sc);
				subRows.setWithoutEventNotifications(sr);
			}
		}
    }

	for (BezierPatch & patch : patches) {
        patch.subdivide(subCols, subRows);
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

	glm::vec2 offset(inputRect.x, inputRect.y);
	glm::vec2 delta;
	delta.s = inputRect.width / cols;
	delta.t = inputRect.height / rows;

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
    getShader().begin();
	mesh.draw();
    getShader().end();
}

//--------------------------------------------------------------
const ofShader & BezierWarper::getShader() const {
    if (!shader.isLoaded()) {
        string fragSource = fragHeader + SoftEdge::getShaderSource() + ColorCorrect::getShaderSource() + fragMain;
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragSource);
        shader.linkProgram();
    }
    return shader;
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
	glm::vec2 * v = vertices->data;
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
	h.vertexIndex = parent->vertexIndex + vertices->width * y + x;
	h.position = vertices->data[h.vertexIndex];
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
	glm::vec2 * v = vertices->data;
	handle.position = (v[handle.vertexIndex] += delta);
}

//--------------------------------------------------------------
void BezierWarper::notifyHandles() {
	updatePatches();
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
		updatePatches();
    }
}

//--------------------------------------------------------------
void BezierWarper::adaptiveSubChanged(int &) {
    if (adaptive) {
        makeSub();
    }
}
