#pragma once

#include "ofMain.h"
#include "Warper.h"
#include "LinearPatch.h"

class LinearWarper : public Warper {
public:
    LinearWarper();

    void setInputRect(shared_ptr<ofRectangle> rect);
    void setVertices(shared_ptr<glm::vec2> vertices, int controlWidth, int controlHeight);

    void update();
	void updateHandles();
	void updateTexCoords();

    void drawGrid();
    void drawSubGrid();
    void drawOutline();
    void drawMesh();
	void drawMesh(SoftEdgePtr softEdge);

    bool select(const glm::vec2 & p);

	void moveHandle(WarpHandle & handle, const glm::vec2 & delta);
	void notifyHandles();

private:
    void makeOutline();
    void makeMesh();
    static void loadShader();
	static void loadShaderSoftEdge();
    
	void setShaderAttributes(ofShader & s);
    float * getVertexPtr(size_t cornerIndex);
    float * getTexCoordPtr(size_t cornerIndex);
    
    shared_ptr<ofRectangle> inputRect;

    size_t controlWidth;
    size_t controlHeight;
    shared_ptr<glm::vec2> vertices;
    
    size_t cols;
    size_t rows;
    vector<LinearPatch> patches;

    ofPolyline outline;

    static ofShader shader;
	static ofShader shaderSoftEdge;
    ofMesh mesh;
};
