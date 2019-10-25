#include "LinearPatch.h"

void LinearPatch::setVertices(const glm::vec2 &topLeft, const glm::vec2 &topRight, const glm::vec2 &bottomRight, const glm::vec2 &bottomLeft) {
    setVertex(0, topLeft);
    setVertex(1, topRight);
    setVertex(2, bottomRight);
    setVertex(3, bottomLeft);
}

void LinearPatch::setVertex(size_t cornerIndex, const glm::vec2 &vertex) {
    for (int i=0; i<4; i++) {
        attribs[i].corners[cornerIndex].vertex = vertex;
    }
}

float * LinearPatch::getVertexPtr(size_t cornerIndex) {
    return &attribs[0].corners[cornerIndex].vertex.x;
}

glm::vec2 & LinearPatch::getVertex(size_t cornerIndex) {
    return attribs[0].corners[cornerIndex].vertex;
}

void LinearPatch::setTexCoords(const glm::vec2 &topLeft, const glm::vec2 &topRight, const glm::vec2 &bottomRight, const glm::vec2 &bottomLeft) {
    setTexCoord(0, topLeft);
    setTexCoord(1, topRight);
    setTexCoord(2, bottomRight);
    setTexCoord(3, bottomLeft);
}

void LinearPatch::setTexCoord(size_t cornerIndex, const glm::vec2 & texCoord) {
    for (int i=0; i<4; i++) {
        attribs[i].corners[cornerIndex].texCoord = texCoord;
    }
}

float * LinearPatch::getTexCoordPtr(size_t cornerIndex) {
    return &attribs[0].corners[cornerIndex].texCoord.x;
}

void LinearPatch::meshVertices(glm::vec3 * vertices) {
    for (int i=0; i<4; i++) {
        vertices[i] = glm::vec3(attribs[0].corners[i].vertex, 0);
    }
}

void LinearPatch::meshVertices(std::vector<glm::vec3> & vertices) {
    vertices.resize(4);
    meshVertices(vertices.data());
}

void LinearPatch::meshTexCoords(glm::vec2 * texCoords, glm::vec2 uv0, glm::vec2 uv1) {
    setTexCoord(0, uv0);
    setTexCoord(1, glm::vec2(uv1.x, uv0.y));
    setTexCoord(2, glm::vec2(uv1.x, uv1.y));
    setTexCoord(3, glm::vec2(uv0.x, uv1.y));
    for (int i=0; i<4; i++) {
        texCoords[i] = attribs[0].corners[i].texCoord;
    }
}

void LinearPatch::meshTexCoords(std::vector<glm::vec2> & texCoords, glm::vec2 uv0, glm::vec2 uv1) {
    texCoords.resize(4);
    meshTexCoords(texCoords.data(), uv0, uv1);
}

unsigned int LinearPatch::meshIndices(unsigned int * indices, unsigned int start) {
    indices[0] = start + 0;
    indices[1] = start + 1;
    indices[2] = start + 2;
    indices[3] = start + 2;
    indices[4] = start + 3;
    indices[5] = start + 0;
    return 4;
}

unsigned int LinearPatch::meshIndices(std::vector<unsigned int> & indices, unsigned int start) {
    indices.resize(6);
    return meshIndices(indices.data(), start);
}
