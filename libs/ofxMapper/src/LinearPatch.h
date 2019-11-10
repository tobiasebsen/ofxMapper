#pragma once

#include "Warper.h"
#include "glm/glm.hpp"
#include <vector>

typedef struct {
    glm::vec2 vertex;
    glm::vec2 texCoord;
} Corner;

typedef struct {
    Corner corners[4];
} CornerVertexAttrib;


class LinearPatch {
public:
    void setVertices(const glm::vec2 & topLeft, const glm::vec2 & topRight, const glm::vec2 & bottomRight, const glm::vec2 & bottomLeft);
    void setVertex(size_t cornerIndex, const glm::vec2 & vertex);
    glm::vec2 & getVertex(size_t cornerIndex);
    float * getVertexPtr(size_t cornerIndex);
    
    void setTexCoords(const glm::vec2 & topLeft, const glm::vec2 & topRight, const glm::vec2 & bottomRight, const glm::vec2 & bottomLeft);
    void setTexCoord(size_t cornerIndex, const glm::vec2 & texCoord);
    float * getTexCoordPtr(size_t cornerIndex);

    void meshVertices(glm::vec3 * vertices);
    void meshVertices(std::vector<glm::vec3> & vertices);
    
    void meshTexCoords(glm::vec2 * texCoords, glm::vec2 uv0, glm::vec2 uv1);
    void meshTexCoords(std::vector<glm::vec2> & texCoords, glm::vec2 uv0, glm::vec2 uv1);
    
    unsigned int meshIndices(unsigned int * indices, unsigned int start = 0);
    unsigned int meshIndices(std::vector<unsigned int> & indices, unsigned int start = 0);

private:
    CornerVertexAttrib attribs[4];
};
