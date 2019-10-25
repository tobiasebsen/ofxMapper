#pragma once

#include "ofMain.h"

/*class WarpHandle;

class WarpControl {
public:

	WarpHandle * handle = NULL;

	glm::vec2 * position = NULL;

	bool selected = false;
	bool dragging = false;
};*/


class WarpHandle {
public:
	/*WarpHandle();

	glm::vec2 getPosition() {
		return *position;
	}
	void setPosition(const glm::vec2 & p);
	void movePosition(const glm::vec2 & delta);

	void operator=(glm::vec2 & p) {
		setPosition(p);
	}
	void operator+=(glm::vec2 & delta) {
		movePosition(delta);
	}

	glm::vec2 * position = NULL;

	union {
		WarpControl controls[4];
		struct {
			WarpControl controlRight;
			WarpControl controlDown;
			WarpControl controlLeft;
			WarpControl controlUp;
		};
	};*/
	glm::vec2 position;
	size_t vertexIndex;
	size_t gridCol = 0;
	size_t gridRow = 0;
	bool selected = false;
	bool dragging = false;
};

