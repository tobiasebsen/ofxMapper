#pragma once

#include "ofMain.h"
#include "DragHandle.h"

class WarpHandle : public DragHandle {
public:

	int vertexIndex = -1;
	int x = -1;
	int y = -1;
	bool isControl = false;

	//size_t gridCol = 0;
	//size_t gridRow = 0;
};

