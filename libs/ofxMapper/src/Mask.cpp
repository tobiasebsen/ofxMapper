#include "Mask.h"

ofTessellator tessellator;

void Mask::setPoints(vector<glm::vec2> &points) {
    poly.clear();
    for (glm::vec2 p : points) {
        poly.addVertex(glm::vec3(p, 0));
    }
    poly.close();
    tessellator.tessellateToMesh(poly, OF_POLY_WINDING_ODD, mesh);
}

vector<glm::vec3> & Mask::getPoints() {
    return poly.getVertices();
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
