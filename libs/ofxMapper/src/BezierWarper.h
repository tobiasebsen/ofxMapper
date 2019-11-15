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

class ControlHandle : public DragHandle {
public:
	int vertexIndex = -1;
	WarpHandle * parent = NULL;
};


class BezierWarper : public Warper, public HasHandlesT<ControlHandle> {
public:

	BezierWarper();
	~BezierWarper();

	void setVertices(shared_ptr<Vertices> vertices);
	void setInputRect(shared_ptr<ofRectangle> inputRect);
    
    VerticesPtr subdivide(int cols, int rows);

    void updatePatches();
    void updateTexCoords();

    void drawGrid();
	void drawSubGrid();
	void drawOutline();
    void drawMesh();
	void drawMesh(SoftEdgePtr softEdge);
    
    bool select(const glm::vec2 & p);

	void moveHandle(WarpHandle & handle, const glm::vec2 & delta);

	void clearHandles();
	void addHandle(WarpHandle * parent, int x, int y);
	void updateHandles(vector<WarpHandle> & handles);
	void moveHandle(ControlHandle & handle, const glm::vec2 & delta);
	void notifyHandles();

	using HasHandlesT<ControlHandle>::moveHandle;

	ofParameter<int> bezierResolution = {"Bezier resolution", 20, 0, 40};
	ofParameter<int> subCols = { "Sub-bezier columns", 20, 0, 40 };
	ofParameter<int> subRows = { "Sub-bezier rows", 20, 0, 40 };
    ofParameter<bool> adaptive = { "Adaptive", true };

    static ofParameter<int> adaptiveBezierRes;
    static ofParameter<int> adaptiveSubRes;

	void adaptiveSubChanged(int&);
    void adaptiveBezierChanged(int&);

private:
	//void makeHandles();
	void makeSub();
	void makeOutline();
    void makeMesh();

    void drawPatch(BezierPatch & patch);
    void drawBezier(Bezier & bezier);
    void drawBeziers(vector<Bezier> & beziers);

	shared_ptr<ofRectangle> inputRect;

	shared_ptr<Vertices> vertices;

    size_t cols;
    size_t rows;
    vector<BezierPatch> patches;

	ofPolyline outline;

    ofMesh mesh;
};
