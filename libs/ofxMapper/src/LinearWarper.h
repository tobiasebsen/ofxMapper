#pragma once

#include "ofMain.h"
#include "Warper.h"
#include "LinearPatch.h"

class LinearWarper : public Warper {
public:
    LinearWarper();

    void setInputRect(ofRectangle & rect);
    void setVertices(VerticesPtr vertices);

    VerticesPtr subdivide(int cols, int rows);

    void updatePatches();
    void updateTexCoords();

    void drawGrid();
    void drawSubGrid();
    void drawOutline();
    void drawMesh();
    
    const ofShader & getShader() const;

    bool select(const glm::vec2 & p);

	void moveHandle(WarpHandle & handle, const glm::vec2 & delta);

private:
    void updatePatchVertices(int col, int row);

    void makeOutline();
    void makeMesh();
    
	void setShaderAttributes(ofShader & s);
	void disableShaderAttributes(ofShader & s);
	float * getVertexPtr(size_t cornerIndex);
    float * getTexCoordPtr(size_t cornerIndex);
    
    ofRectangle inputRect;
    VerticesPtr vertices;
    
    size_t cols;
    size_t rows;
    vector<LinearPatch> patches;

    ofPolyline outline;

    static ofShader shader;
    ofMesh mesh;
};
