#include "BezierPatch.h"

bool BezierPatch::tensorPatch = false;

//--------------------------------------------------------------
void BezierPatch::moveTopLeft(glm::vec2 & delta) {
    bezierRows[0].moveStart(delta);
    bezierCols[0].moveStart(delta);
}

//--------------------------------------------------------------
void BezierPatch::moveTopRight(glm::vec2 & delta) {
	bezierRows[0].moveEnd(delta);
	bezierCols[3].moveStart(delta);
}

//--------------------------------------------------------------
void BezierPatch::moveBottomLeft(glm::vec2 & delta) {
    bezierRows[3].moveStart(delta);
	bezierCols[0].moveEnd(delta);
}

//--------------------------------------------------------------
void BezierPatch::moveBottomRight(glm::vec2 & delta) {
	bezierRows[3].moveEnd(delta);
	bezierCols[3].moveEnd(delta);
}

//--------------------------------------------------------------
void BezierPatch::setResolution(size_t resolution) {
	bezierRows[0].setResolution(resolution);
	bezierCols[0].setResolution(resolution);
	bezierCols[3].setResolution(resolution);
	bezierRows[3].setResolution(resolution);
    for (Bezier & b : bezierSubRows)
        b.setResolution(resolution);
    for (Bezier & b : bezierSubCols)
        b.setResolution(resolution);
}

//--------------------------------------------------------------
void BezierPatch::subdivide(size_t subdivRows, size_t subdivCols) {
	makeSubdiv(bezierRows[0], bezierRows[3], bezierCols[0], bezierCols[3], bezierRows[1], bezierRows[2], bezierSubCols, subdivCols);
	makeSubdiv(bezierCols[0], bezierCols[3], bezierRows[0], bezierRows[3], bezierCols[1], bezierCols[2], bezierSubRows, subdivRows);
}

//--------------------------------------------------------------
void BezierPatch::meshVertices(glm::vec3 * vertices) {
	size_t gridRows = bezierSubRows.size();
	size_t gridCols = bezierSubCols.size();
	size_t subdivhCols = bezierSubCols.size() - 2;

	size_t rowIndex = 0;
	for (size_t r = 0; r < gridRows; r++) {
		bezierSubRows[r].getResampled(subdivhCols, vertices + rowIndex);
		rowIndex += gridCols;
	}
}

//--------------------------------------------------------------
void BezierPatch::meshVertices(std::vector<glm::vec3>& vertices) {
	size_t gridCols = bezierSubCols.size();
	size_t gridRows = bezierSubRows.size();
	vertices.resize(gridCols * gridRows);
	meshVertices(vertices.data());
}

//--------------------------------------------------------------
void BezierPatch::meshTexCoords(glm::vec2 * texCoords, glm::vec2 uv0, glm::vec2 uv1) {
	size_t gridCols = bezierSubCols.size();
	size_t gridRows = bezierSubRows.size();
	size_t quadCols = gridCols - 1;
	size_t quadRows = gridRows - 1;
	glm::vec2 delta(1.f / quadCols, 1.f / quadRows);

	size_t rowIndex = 0;
	for (size_t r = 0; r < gridRows; r++) {
		for (size_t c = 0; c < gridCols; c++) {
			glm::vec2 uv;
			uv.s = c * delta.s;
			uv.t = r * delta.t;
			texCoords[rowIndex+c] = uv0 + (uv1 - uv0) * uv;
		}
		rowIndex += gridCols;
	}
}

//--------------------------------------------------------------
void BezierPatch::meshTexCoords(std::vector<glm::vec2> & texCoords, glm::vec2 uv0, glm::vec2 uv1) {
	size_t gridCols = bezierSubCols.size();
	size_t gridRows = bezierSubRows.size();
	texCoords.resize(gridCols * gridRows);
	meshTexCoords(texCoords.data(), uv0, uv1);
}

//--------------------------------------------------------------
unsigned int BezierPatch::meshIndices(unsigned int * indices, unsigned int start) {
	size_t gridCols = bezierSubCols.size();
	size_t gridRows = bezierSubRows.size();
	size_t quadCols = gridCols - 1;
	size_t quadRows = gridRows - 1;

	size_t quadIndex = 0;
	for (size_t r = 0; r < quadRows; r++) {
		for (size_t c = 0; c < quadCols; c++) {
			size_t vertexIndex = start + r * gridCols + c;
			indices[quadIndex + 0] = vertexIndex + 0;
			indices[quadIndex + 1] = vertexIndex + 1;
			indices[quadIndex + 2] = vertexIndex + 1 + gridCols;
			indices[quadIndex + 3] = vertexIndex + 1 + gridCols;
			indices[quadIndex + 4] = vertexIndex + 0 + gridCols;
			indices[quadIndex + 5] = vertexIndex + 0;
			quadIndex += 6;
		}
	}
	return gridCols * gridRows;
}

//--------------------------------------------------------------
unsigned int BezierPatch::meshIndices(std::vector<unsigned int>& indices, unsigned int start) {
	size_t quadCols = bezierSubCols.size() - 1;
	size_t quadRows = bezierSubRows.size() - 1;
	indices.resize(quadCols * quadRows * 6);
	return meshIndices(indices.data(), start);
}

//--------------------------------------------------------------
void BezierPatch::makeSubdiv(Bezier & a1, Bezier & a2, Bezier & b1, Bezier & b2, Bezier & c1, Bezier & c2, std::vector<Bezier> & beziers, size_t subdiv) {

    std::vector<glm::vec2> vertA;
    std::vector<glm::vec2> vertB;
    
    a1.getResampled(subdiv, vertA);
    a2.getResampled(subdiv, vertB);

    size_t n = vertA.size();
    beziers.resize(n);

	if (tensorPatch) {
		std::vector<glm::vec2> vertC1;
		std::vector<glm::vec2> vertC2;

		c1.getResampled(subdiv, vertC1);
		c2.getResampled(subdiv, vertC2);

		for (size_t i = 0; i < n; i++) {
			float f = ((float)i) / ((float)(n - 1));
			glm::vec2 a = vertA[i];
			glm::vec2 b = vertB[i];
			glm::vec2 ac = vertC1[i];
			glm::vec2 bc = vertC2[i];
			beziers[i].set(a, ac, bc, b);
		}
	}
	else {
		for (size_t i = 0; i < n; i++) {
			float f = ((float)i) / ((float)(n - 1));
			glm::vec2 a = vertA[i];
			glm::vec2 b = vertB[i];
			glm::vec2 ac = glm::mix(b1.getC1(), b2.getC1(), f) + a;
			glm::vec2 bc = glm::mix(b1.getC2(), b2.getC2(), f) + b;
			beziers[i].set(a, ac, bc, b);
		}
	}
}
