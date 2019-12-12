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

		bool selectInput(const glm::vec2 & p);
		void moveInput(const glm::vec2 & delta);

		void updateInputHandles();
		vector<RectHandle> & getInputHandles();

		bool grabInputHandle(const glm::vec2 & p, float radius);
        void setInputHandle(const glm::vec2 & p);
        void setInputHandle(RectHandle & handle, const glm::vec2 & p);
		void dragInputHandle(const glm::vec2 & delta);
		bool moveInputHandle(const glm::vec2 & delta);
		void moveInputHandle(RectHandle & handle, const glm::vec2 & delta);
		void releaseInputHandle();

		// Vertices
		void setVertices(vector<glm::vec2> & vertices, size_t controlWidth, size_t controlHeight);
		void createVertices(const ofRectangle & rect);
		void createVertices(const glm::vec2 & topLeft, const glm::vec2 & topRight, const glm::vec2 & bottomRight, const glm::vec2 & bottomLeft);
		glm::vec2 * getVertices();
		size_t getControlWidth();
		size_t getControlHeight();
		void subdivide(int cols, int rows);
		void reset();

		// Draw
		virtual void draw();
		virtual void drawOutline();

		virtual glm::vec2 getCenter();

		virtual bool select(const glm::vec2 & p);
		virtual void move(const glm::vec2 & delta);


		// Warper
		void update();
		Warper * getWarper();
		BezierWarper & getBezierWarper();


		// Handles
		void updateHandles();
		bool grabHandle(const glm::vec2 & p, float radius);
		void dragHandle(const glm::vec2 & delta);
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

	public:

		// Parameters
		ofParameter<int> inputX = { "X", 0, 0, 3840 };
		ofParameter<int> inputY = { "Y", 0, 0, 3840 };
		ofParameter<int> inputWidth = { "Width", 1920, 0, 3840 };
		ofParameter<int> inputHeight = { "Height", 1080, 0, 3840 };
		ofParameter<bool> softEdgeEnabled = { "Soft edge", false };
		ofParameter<bool> dragging = { "Dragging", false };
		ofParameterGroup inputGroup = { "Slice", name, inputX, inputY, inputWidth, inputHeight, softEdgeEnabled, enabled, remove };

		ofParameter<bool> bezierEnabled = { "Bezier", false };
		ofParameter<bool> colorEnabled = { "Color correction", false };
		ofParameterGroup warperGroup = { "Warper", name, enabled, editEnabled, bezierEnabled, colorEnabled, softEdgeEnabled };

	private:
        friend class Screen;
        Slice(float x, float y, float width, float height);

		void inputRectChanged(int&);
		void bezierChanged(bool&);

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
