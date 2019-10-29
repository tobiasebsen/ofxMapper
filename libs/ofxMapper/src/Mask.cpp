#include "Mask.h"

ofTessellator tessellator;

void Mask::setPoints(vector<glm::vec2> &points) {
	handles.clear();
    for (glm::vec2 & p : points) {
		DragHandle h;
		h.position = p;
		handles.push_back(h);
    }
	update();
}

void Mask::update() {
	poly.clear();
	for (DragHandle & h : handles) {
		poly.addVertex(glm::vec3(h.position, 0));
	}
	poly.close();
	tessellator.tessellateToMesh(poly, OF_POLY_WINDING_ODD, mesh);
}

void Mask::draw() {
    mesh.draw();
}

void Mask::drawOutline() {
    poly.draw();
}

bool Mask::select(const glm::vec2 &p) {
    selected = poly.inside(p.x, p.y);
    return selected;
}

void Mask::moveHandle(DragHandle & handle, const glm::vec2 & delta) {
	handle.position += delta;
}

void Mask::notifyHandles() {
	update();
}
