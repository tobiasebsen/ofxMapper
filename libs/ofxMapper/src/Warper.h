#pragma once

#include "ofMain.h"
#include "Vertices.h"
#include "WarpHandle.h"
#include "SoftEdge.h"


class Warper {
public:
	virtual void setInputRect(ofRectangle & rect) = 0;
	virtual void setVertices(VerticesPtr vertices) = 0;

    virtual VerticesPtr subdivide(int cols, int rows) = 0;

    virtual void updatePatches() = 0;
    virtual void updateTexCoords() = 0;

	virtual void drawGrid() = 0;
	virtual void drawSubGrid() = 0;
	virtual void drawOutline() = 0;
	virtual void drawMesh() = 0;

    virtual const ofShader & getShader() const = 0;

	virtual bool select(const glm::vec2 & p) = 0;

	virtual void moveHandle(WarpHandle & handle, const glm::vec2 & delta) = 0;
};
