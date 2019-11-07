#pragma once

#include "ofMain.h"
#include "DragHandle.h"

class WarpHandle : public DragHandle {
public:

	int vertexIndex = -1;
	size_t col = -1;
	size_t row = -1;
};
