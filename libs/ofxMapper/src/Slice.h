#pragma once

#include "ofMain.h"
#include "Element.h"
#include "Vertices.h"
#include "Warper.h"
#include "WarpHandle.h"
#include "BezierWarper.h"
#include "LinearWarper.h"
#include "SoftEdge.h"
#include "ColorCorrect.h"

class RectHandle : public DragHandle {
public:
	enum {
		SIDE_TOP, SIDE_RIGHT, SIDE_BOTTOM, SIDE_LEFT
	} side;
};

namespace ofxMapper {

	class Slice : public Element, public HasHandlesT<WarpHandle> {
	public:
		~Slice();

		// Input rectangle
		void setInputRect(const ofRectangle & inputRect);
		ofRectangle getInputRect();
		void drawInputRect();
		void updateInputHandles();
		vector<RectHandle> & getInputHandles();
		bool grabInputHandle(const glm::vec2 & p, float radius);
		bool dragInputHandle(const glm::vec2 & delta);
		void moveInputHandle(RectHandle & handle, const glm::vec2 & delta);
		void releaseInputHandle();

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

		virtual glm::vec2 getCenter();

		bool select(const glm::vec2 & p);
		bool selectInput(const glm::vec2 & p);


		// Warper
		void update();
		Warper * getWarper();
		BezierWarper & getBezierWarper();


		// Handles
		void updateHandles();
		bool grabHandle(const glm::vec2 & p, float radius);
		bool dragHandle(const glm::vec2 & delta);
		bool moveHandle(const glm::vec2 & delta);
		void moveHandle(WarpHandle & handle, const glm::vec2 & delta);
		void notifyHandles();

		using HasHandlesT<WarpHandle>::grabHandle;
		using HasHandlesT<WarpHandle>::moveHandle;
		using HasHandlesT<WarpHandle>::dragHandle;

		// Bland rects
		void clearBlendRects();
		bool addBlendRect(const ofRectangle & rect);
		vector<ofRectangle> & getBlendRects();


		// Soft edge
		SoftEdge & getSoftEdge();
    
        // Color correction
        ColorCorrect & getColorCorrect();


		// Parameters
		ofParameter<int> inputX = { "X", 0, 0, 1920 };
		ofParameter<int> inputY = { "Y", 0, 0, 1080 };
		ofParameter<int> inputWidth = { "Width", 1920, 0, 1920 };
		ofParameter<int> inputHeight = { "Height", 1080, 0, 1080 };
		ofParameter<bool> softEdgeEnabled = { "Soft edge", false };
		ofParameter<bool> dragging = { "Dragging", false };
		ofParameterGroup inputGroup = { "Slice", name, inputX, inputY, inputWidth, inputHeight, softEdgeEnabled, enabled, remove };

		ofParameter<bool> bezierEnabled = { "Bezier", false };
		ofParameter<bool> colorEnabled = { "Color correction", false };
		ofParameterGroup warperGroup = { "Warper", name, enabled, editEnabled, bezierEnabled, colorEnabled, softEdgeEnabled };

		void inputRectChanged(int&);
		void bezierChanged(bool&);

	private:
        friend class Screen;
        Slice(float x, float y, float width, float height);

		vector<RectHandle> inputHandles;

		VerticesPtr vertices;

		Warper * warper;
		BezierWarper bezierWarper;
		LinearWarper linearWarper;

		vector<ofRectangle> blendRects;

		SoftEdge softEdge;
        ColorCorrect colorCorrect;
	};

	typedef shared_ptr<Slice> SlicePtr;
}
