#pragma once

#include "ofMain.h"
#include "Warper.h"
#include "WarpHandle.h"
#include "BezierWarper.h"
#include "LinearWarper.h"
#include "SoftEdge.h"


class Slice {
public:
	Slice();
	~Slice();

	void setInputRect(ofRectangle inputRect);
	ofRectangle getInputRect();

	void setVertices(vector<glm::vec2> & vertices, size_t controlWidth, size_t controlHeight);
	void createVertices(const ofRectangle & rect);

	void update();

	// Draw
	void drawInputRect();
	void drawGrid();
	void drawSubGrid();
	void drawMesh();

	bool selectInput(const glm::vec2 & p);
	bool selectWarper(const glm::vec2 & p);


	Warper * getWarper();


	// Handles
	//void updateHandles();
	/*size_t getNumHandles() const;
	DragHandle * getHandle(size_t);
	const DragHandle * getHandle(size_t) const;
	bool selectHandle(const glm::vec2 & p, float radius);
	bool grabHandle(const glm::vec2 & p, float radius);
	bool dragHandle(const glm::vec2 & delta) = 0;
	void releaseHandle() = 0;*/
	//void moveHandle(WarpHandle & handle, const glm::vec2 & delta);
    
    
	// Bland rects
    void clearBlendRects();
    bool addBlendRect(const ofRectangle & rect);
    vector<ofRectangle> & getBlendRects();


	// Soft edge
	SoftEdgePtr getSoftEdge();


	ofParameter<string> name = { "Name:", "" };
	ofParameter<int> inputX = { "X", 0, 0, 1920 };
	ofParameter<int> inputY = { "Y", 0, 0, 1080 };
	ofParameter<int> inputWidth = { "Width", 1920, 0, 1920 };
	ofParameter<int> inputHeight = { "Height", 1080, 0, 1080 };
	ofParameter<bool> softEdgeEnabled = { "Soft edge", false };
	ofParameter<bool> enabled = { "Enabled", true };
	ofParameter<bool> remove = { "Remove", false };
	ofParameter<bool> selected = { "Selected", false };
	ofParameter<bool> dragging = { "Dragging", false };
	ofParameterGroup inputGroup = { "Slice", name, inputX, inputY, inputWidth, inputHeight, softEdgeEnabled, enabled, remove };

    ofParameter<bool> editEnabled = { "Edit", false };
	ofParameter<bool> bezierEnabled = { "Bezier", false };
	ofParameterGroup warperGroup = {"Warper", name, enabled, editEnabled, bezierEnabled, softEdgeEnabled };

	void inputRectChanged(int&);
    void bezierChanged(bool&);

private:
	shared_ptr<ofRectangle> inputRect;

	size_t controlWidth;
	size_t controlHeight;
	shared_ptr<glm::vec2> vertices;

	size_t gridCols;
	size_t gridRows;
	//vector<WarpHandle> handles;

	Warper * warper;
	BezierWarper bezierWarper;
	LinearWarper linearWarper;

    vector<ofRectangle> blendRects;

	SoftEdgePtr softEdge;
};

typedef shared_ptr<Slice> SlicePtr;
