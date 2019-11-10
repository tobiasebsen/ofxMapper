#pragma once

#include "ofMain.h"
#include "Element.h"
#include "Vertices.h"
#include "Warper.h"
#include "WarpHandle.h"
#include "BezierWarper.h"
#include "LinearWarper.h"
#include "SoftEdge.h"


class Slice : public Element, public HasHandlesT<WarpHandle> {
public:
	Slice();
	~Slice();

    // Input rectangle
	void setInputRect(ofRectangle inputRect);
	ofRectangle getInputRect();

    // Vertices
	void setVertices(vector<glm::vec2> & vertices, size_t controlWidth, size_t controlHeight);
	void createVertices(const ofRectangle & rect);
	glm::vec2 * getVertices();
	size_t getControlWidth();
	size_t getControlHeight();
    void subdivide(int cols, int rows);

	// Draw
	virtual void draw();
	virtual void drawOutline();
	void drawInputRect();
	void drawGrid();
	void drawSubGrid();

	bool selectInput(const glm::vec2 & p);
	bool selectWarper(const glm::vec2 & p);


    // Warper
    void update();
	Warper * getWarper();
	BezierWarper & getBezierWarper();


	// Handles
	void updateHandles();
	bool grabHandle(const glm::vec2 & p, float radius);
	bool dragHandle(const glm::vec2 & delta);
	void moveHandle(WarpHandle & handle, const glm::vec2 & delta);
	void notifyHandles();
    
    
	// Bland rects
    void clearBlendRects();
    bool addBlendRect(const ofRectangle & rect);
    vector<ofRectangle> & getBlendRects();


	// Soft edge
	SoftEdgePtr getSoftEdge();


	//ofParameter<string> name = { "Name:", "" };
	ofParameter<int> inputX = { "X", 0, 0, 1920 };
	ofParameter<int> inputY = { "Y", 0, 0, 1080 };
	ofParameter<int> inputWidth = { "Width", 1920, 0, 1920 };
	ofParameter<int> inputHeight = { "Height", 1080, 0, 1080 };
	ofParameter<bool> softEdgeEnabled = { "Soft edge", false };
	//ofParameter<bool> enabled = { "Enabled", true };
	//ofParameter<bool> remove = { "Remove", false };
	//ofParameter<bool> selected = { "Selected", false };
	ofParameter<bool> dragging = { "Dragging", false };
	ofParameterGroup inputGroup = { "Slice", name, inputX, inputY, inputWidth, inputHeight, softEdgeEnabled, enabled, remove };

    //ofParameter<bool> editEnabled = { "Edit", false };
	ofParameter<bool> bezierEnabled = { "Bezier", false };
    ofParameter<bool> colorEnabled = { "Color correction", false };
	ofParameterGroup warperGroup = {"Warper", name, enabled, editEnabled, bezierEnabled, colorEnabled, softEdgeEnabled };
    
    ofParameter<float> colorBrightness = {"Brightness", 0, -1, 1};
    ofParameter<float> colorContrast = {"Contrast", 0, -1, 1};
    ofParameter<float> colorRed = {"Red", 0, -1, 1};
    ofParameter<float> colorGreen = {"Green", 0, -1, 1};
    ofParameter<float> colorBlue = {"Blue", 0, -1, 1};
    ofParameterGroup colorGroup = {"Color correction", colorBrightness, colorContrast, colorRed, colorGreen, colorBlue};

	void inputRectChanged(int&);
    void bezierChanged(bool&);

private:
	shared_ptr<ofRectangle> inputRect;

	shared_ptr<Vertices> vertices;

	Warper * warper;
	BezierWarper bezierWarper;
	LinearWarper linearWarper;

    vector<ofRectangle> blendRects;

	SoftEdgePtr softEdge;
};

typedef shared_ptr<Slice> SlicePtr;
