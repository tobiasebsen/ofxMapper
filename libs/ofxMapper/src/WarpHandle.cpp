#include "WarpHandle.h"

/*WarpHandle::WarpHandle() {
	position = NULL;
	for (WarpControl & c : controls) {
		c.handle = this;
		c.position = NULL;
		c.selected = false;
		c.dragging = false;
	}
}

void WarpHandle::setPosition(const glm::vec2 & p) {
	glm::vec2 delta = p - *position;
	movePosition(delta);
}

void WarpHandle::movePosition(const glm::vec2 & delta) {
	*position += delta;
	for (WarpControl & c : controls) {
		if (c.position) *c.position += delta;
	}
}
*/