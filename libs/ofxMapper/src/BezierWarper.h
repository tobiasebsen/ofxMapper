#pragma once

#include "ofMain.h"
#include "Warper.h"
#include "WarpHandle.h"
#include "Bezier.h"
#include "BezierPatch.h"

typedef struct {
    BezierPatch * topLeft = NULL;
    BezierPatch * bottomLeft = NULL;
    BezierPatch * topRight = NULL;
    BezierPatch * bottomRight = NULL;
} PatchJunction;

class BezierWarper : public Warper {
public:

	BezierWarper();
	~BezierWarper();

	void setVertices(shared_ptr<glm::vec2> vertices, int controlWidth, int controlHeight);
	void setInputRect(shared_ptr<ofRectangle> inputRect);

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


	ofParameter<int> bezierResolution = {"Bezier resolution", 20, 0, 40};
	ofParameter<int> subdivCols = { "Subdivide horizontal", 20, 0, 40 };
	ofParameter<int> subdivRows = { "Subdivide vertical", 20, 0, 40 };
    ofParameter<bool> adaptive = { "Adaptive", true };

    static ofParameter<int> adaptiveBezierRes;
    static ofParameter<int> adaptiveSubdivRes;

	void adaptiveSubdivChanged(int&);
    void adaptiveBezierChanged(int&);

private:
	//void makeHandles();
	void makeSubdiv();
	void makeOutline();
    void makeMesh();

    void drawPatch(BezierPatch & patch);
    void drawBezier(Bezier & bezier);
    void drawBeziers(vector<Bezier> & beziers);

	shared_ptr<ofRectangle> inputRect;

	size_t controlWidth;
	size_t controlHeight;
	shared_ptr<glm::vec2> vertices;

    size_t cols;
    size_t rows;
    vector<BezierPatch> patches;

	ofPolyline outline;

    ofMesh mesh;
};
