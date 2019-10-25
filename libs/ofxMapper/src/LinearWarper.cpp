#include "LinearWarper.h"
#include "LinearShader.h"

ofShader LinearWarper::shader;
ofShader LinearWarper::shaderSoftEdge;

LinearWarper::LinearWarper() {
    inputRect = shared_ptr<ofRectangle>(new ofRectangle);
    rows = 0;
    cols = 0;
    controlWidth = 0;
    controlHeight = 0;
}

//--------------------------------------------------------------
void LinearWarper::setInputRect(shared_ptr<ofRectangle> rect) {
    inputRect = rect;
}

//--------------------------------------------------------------
void LinearWarper::setVertices(shared_ptr<glm::vec2> vertices, int controlWidth, int controlHeight) {

    this->controlWidth = controlWidth;
    this->controlHeight = controlHeight;
    this->vertices = vertices;
    
    rows = (controlHeight - 1) / 3;
    cols = (controlWidth - 1) / 3;
    
    update();
    updateTexCoords();
}

//--------------------------------------------------------------
void LinearWarper::update() {

    patches.resize(rows * cols);
    
    glm::vec2 * v = vertices.get();
    
    size_t rowIndex = 0;
    for (size_t r=0; r<rows; r++) {
        for (size_t c=0; c<cols; c++) {
            LinearPatch & patch = patches[rowIndex+c];

            size_t itl = r * controlWidth * 3 + c * 3;
            size_t itr = itl + 3;
            size_t ibr = itr + controlWidth * 3;
            size_t ibl = ibr - 3;

            patch.setVertices(v[itl], v[itr], v[ibr], v[ibl]);
        }
        rowIndex += cols;
    }
    
    makeOutline();
    makeMesh();
}

//--------------------------------------------------------------
void LinearWarper::updateTexCoords() {
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

            LinearPatch & patch = patches[r * cols + c];

            patch.meshTexCoords(texCoords, uv0, uv1);
            mesh.addTexCoords(texCoords);
        }
    }
}

//--------------------------------------------------------------
void LinearWarper::drawGrid() {
    for (LinearPatch & patch : patches) {
        glBegin(GL_LINE_LOOP);
        glVertex2fv(&patch.getVertex(0).x);
        glVertex2fv(&patch.getVertex(1).x);
        glVertex2fv(&patch.getVertex(2).x);
        glVertex2fv(&patch.getVertex(3).x);
        glEnd();
    }
}

//--------------------------------------------------------------
void LinearWarper::drawSubGrid() {
    drawGrid();
}

//--------------------------------------------------------------
void LinearWarper::drawOutline() {
	outline.draw();
}

//--------------------------------------------------------------
void LinearWarper::drawMesh() {
    
    if (!shader.isLoaded())
        loadShader();

    shader.begin();
	setShaderAttributes(shader);

    ofGetCurrentRenderer()->draw(mesh, OF_MESH_FILL, false, false, false);
    
    shader.end();
}

void LinearWarper::setShaderAttributes(ofShader & s) {
	s.setAttribute2fv("c1", getVertexPtr(0), sizeof(CornerVertexAttrib));
	s.setAttribute2fv("c2", getVertexPtr(1), sizeof(CornerVertexAttrib));
	s.setAttribute2fv("c3", getVertexPtr(2), sizeof(CornerVertexAttrib));
	s.setAttribute2fv("c4", getVertexPtr(3), sizeof(CornerVertexAttrib));
	s.setAttribute2fv("t1", getTexCoordPtr(0), sizeof(CornerVertexAttrib));
	s.setAttribute2fv("t2", getTexCoordPtr(1), sizeof(CornerVertexAttrib));
	s.setAttribute2fv("t3", getTexCoordPtr(2), sizeof(CornerVertexAttrib));
	s.setAttribute2fv("t4", getTexCoordPtr(3), sizeof(CornerVertexAttrib));
}

void LinearWarper::drawMesh(SoftEdgePtr softEdge) {

	if (!shaderSoftEdge.isLoaded())
		loadShaderSoftEdge();

	shaderSoftEdge.begin();
	setShaderAttributes(shaderSoftEdge);
	softEdge->setUniforms(shaderSoftEdge, inputRect.get());

	ofGetCurrentRenderer()->draw(mesh, OF_MESH_FILL, false, false, false);

	shaderSoftEdge.end();
}

//--------------------------------------------------------------
bool LinearWarper::select(const glm::vec2 & p) {
    return outline.size() > 0 && outline.inside(p.x, p.y);
}

void LinearWarper::moveVertex(size_t gridCol, size_t gridRow, const glm::vec2 & delta) {
	glm::vec2 * v = vertices.get();
	v[gridRow * controlWidth * 3 + gridCol * 3] += delta;
	update();
}


//--------------------------------------------------------------
void LinearWarper::makeOutline() {

    size_t rowIndex;
    
    outline.clear();
    
    // Top
    for (size_t c = 0; c < cols; c++) {
        glm::vec2 & v = patches[c].getVertex(0);
        outline.addVertex(glm::vec3(v,0));
    }
    // Right
    rowIndex = cols - 1;
    for (size_t r = 0; r < rows; r++) {
        glm::vec2 & v = patches[rowIndex].getVertex(1);
        outline.addVertex(glm::vec3(v,0));
        rowIndex += cols;
    }
    // Bottom
    rowIndex = (rows - 1) * cols;
    for (int c = cols-1; c >= 0; c--) {
        glm::vec2 & v = patches[rowIndex + c].getVertex(2);
        outline.addVertex(glm::vec3(v,0));
    }
    // Left
    rowIndex = (rows - 1) * cols;
    for (int r = rows-1; r >= 0; r--) {
        glm::vec2 v = patches[rowIndex].getVertex(3);
        outline.addVertex(glm::vec3(v,0));
        rowIndex -= cols;
    }
    outline.close();
}

//--------------------------------------------------------------
void LinearWarper::makeMesh() {
    mesh.clearVertices();
    mesh.clearIndices();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    vector<glm::vec3> vertices;
    vector<unsigned int> indices;
    
    unsigned int offsetIndex = 0;
    
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            
            LinearPatch & patch = patches[r * cols + c];
            
            patch.meshVertices(vertices);
            mesh.addVertices(vertices);
            
            offsetIndex += patch.meshIndices(indices, offsetIndex);
            mesh.addIndices(indices);
        }
    }
}

//--------------------------------------------------------------
void LinearWarper::loadShader() {
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertSource);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragHeader + quadCoordFrag + quadCoordMain);
    shader.linkProgram();
}

void LinearWarper::loadShaderSoftEdge() {
	string fragSource = fragHeader + SoftEdge::getShaderSource() + quadCoordFrag + quadCoordSoftEdgeMain;
	shaderSoftEdge.setupShaderFromSource(GL_VERTEX_SHADER, vertSource);
	shaderSoftEdge.setupShaderFromSource(GL_FRAGMENT_SHADER, fragSource);
	shaderSoftEdge.linkProgram();
}

//--------------------------------------------------------------
inline float * LinearWarper::getVertexPtr(size_t cornerIndex) {
    return patches[0].getVertexPtr(cornerIndex);
}

//--------------------------------------------------------------
inline float * LinearWarper::getTexCoordPtr(size_t cornerIndex) {
    return patches[0].getTexCoordPtr(cornerIndex);
}
