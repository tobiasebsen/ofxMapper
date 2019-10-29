#pragma once

#include "ofMain.h"
#include "WarpHandle.h"
#include "SoftEdge.h"

class Warper : public HasHandlesT<WarpHandle> {
public:
	virtual void setInputRect(shared_ptr<ofRectangle> rect) = 0;
	virtual void setVertices(shared_ptr<glm::vec2> vertices, int controlWidth, int controlHeight) = 0;

	virtual void update() = 0;
	virtual void updateTexCoords() = 0;

	virtual void drawGrid() = 0;
	virtual void drawSubGrid() = 0;
	virtual void drawOutline() = 0;
	virtual void drawMesh() = 0;
	virtual void drawMesh(SoftEdgePtr softEdge) = 0;

	virtual bool select(const glm::vec2 & p) = 0;

	virtual void moveHandle(WarpHandle & handle, const glm::vec2 & delta) = 0;
	virtual void notifyHandles() = 0;
};
