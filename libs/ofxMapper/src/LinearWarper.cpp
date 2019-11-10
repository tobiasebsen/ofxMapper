#include "LinearWarper.h"
#include "LinearShader.h"

ofShader LinearWarper::shader;
ofShader LinearWarper::shaderSoftEdge;

//--------------------------------------------------------------
LinearWarper::LinearWarper() {
    inputRect = shared_ptr<ofRectangle>(new ofRectangle);
    rows = 0;
    cols = 0;
}

//--------------------------------------------------------------
void LinearWarper::setInputRect(shared_ptr<ofRectangle> rect) {
    inputRect = rect;
    updateTexCoords();
}

//--------------------------------------------------------------
void LinearWarper::setVertices(shared_ptr<Vertices> v) {

    vertices = v;

    cols = (vertices->width - 1) / 3;
    rows = (vertices->height - 1) / 3;
    
    updatePatches();
    updateTexCoords();
}

//--------------------------------------------------------------
VerticesPtr LinearWarper::subdivide(int subdivCols, int subdivRows) {

    int subWidth = (vertices->width-1) * subdivCols + 1;
    int subHeight = (vertices->height-1) * subdivRows + 1;
    shared_ptr<Vertices> subvertices = shared_ptr<Vertices>(new Vertices(subWidth, subHeight));
    
    int subdivColStride = subdivCols * 3;
    int subdivRowStride = subdivRows * 3;
    float dx = 1.f/subdivColStride;
    float dy = 1.f/subdivRowStride;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int i = r * vertices->width * 3 + c * 3;
            glm::vec2 v00 = vertices->data[i];
            glm::vec2 v10 = vertices->data[i+3];
            glm::vec2 v01 = vertices->data[i+vertices->width*3];
            glm::vec2 v11 = vertices->data[i+vertices->width*3+3];
            int row = r * subdivRows * 3;
            int col = c * subdivCols * 3;
            
            for (int y=0; y<=subdivRowStride; y++) {
                float fy = y * dy;
                glm::vec2 w0 = glm::mix(v00, v01, fy);
                glm::vec2 w1 = glm::mix(v10, v11, fy);

                for (int x=0; x<=subdivColStride; x++) {
                    float fx = x * dx;
                    int j = (row + y) * subWidth + (col + x);
                    subvertices->data[j] = glm::mix(w0, w1, fx);
                }
            }
        }
    }
    return subvertices;
}

//--------------------------------------------------------------
void LinearWarper::updatePatches() {
    
    patches.resize(rows * cols);
    
    glm::vec2 * v = vertices->data;
    
    size_t rowIndex = 0;
    for (size_t r=0; r<rows; r++) {
        for (size_t c=0; c<cols; c++) {
            LinearPatch & patch = patches[rowIndex+c];
            
            size_t itl = r * vertices->width * 3 + c * 3;
            size_t itr = itl + 3;
            size_t ibr = itr + vertices->width * 3;
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

//--------------------------------------------------------------
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

//--------------------------------------------------------------
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

//--------------------------------------------------------------
void LinearWarper::moveHandle(WarpHandle & handle, const glm::vec2 & delta) {
	glm::vec2 * v = vertices->data;
	handle.position = (v[handle.vertexIndex] += delta);

    if (handle.col < cols) {
        if (handle.row > 0)
            updatePatchVertices(handle.col, handle.row-1);
        if (handle.row < rows)
            updatePatchVertices(handle.col, handle.row);
    }
    if (handle.col > 0) {
        if (handle.row > 0)
            updatePatchVertices(handle.col-1, handle.row-1);
        if (handle.row < rows)
            updatePatchVertices(handle.col-1, handle.row);
    }
}

//--------------------------------------------------------------
void LinearWarper::updatePatchVertices(int col, int row) {
    int c1 = col * 3;
    int c2 = (col+1) * 3;
    int r1 = row * 3;
    int r2 = (row+1) * 3;
    int stride = vertices->width;

    glm::vec2 * v = vertices->data;
    glm::vec2 v00 = v[r1*stride+c1];
    glm::vec2 v10 = v[r1*stride+c2];
    glm::vec2 v01 = v[r2*stride+c1];
    glm::vec2 v11 = v[r2*stride+c2];
    
    for (int r=0; r<4; r++) {
        float fr = r/3.f;
        glm::vec2 w0 = glm::mix(v00, v01, fr);
        glm::vec2 w1 = glm::mix(v10, v11, fr);

        for (int c=0; c<4; c++) {
            float fc = c/3.f;
            int index = (r1 + r) * stride + (c1 + c);

            v[index] = glm::mix(w0, w1, fc);
        }
    }
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
    return patches.size() > 0 ? patches[0].getVertexPtr(cornerIndex) : NULL;
}

//--------------------------------------------------------------
inline float * LinearWarper::getTexCoordPtr(size_t cornerIndex) {
    return patches.size() > 0 ? patches[0].getTexCoordPtr(cornerIndex) : NULL;
}
