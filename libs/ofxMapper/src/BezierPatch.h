#pragma once

#include "Bezier.h"
#include <vector>

class BezierPatch {
public:
    
    void moveTopLeft(glm::vec2 & delta);
    void moveTopRight(glm::vec2 & delta);
    void moveBottomLeft(glm::vec2 & delta);
    void moveBottomRight(glm::vec2 & delta);

    void setResolution(size_t resolution);
    void subdivide(size_t subdivRows, size_t subdivCols);

	void meshVertices(glm::vec3 * vertices);
	void meshVertices(std::vector<glm::vec3> & vertices);

	void meshTexCoords(glm::vec2 * texCoords, glm::vec2 uv0, glm::vec2 uv1);
	void meshTexCoords(std::vector<glm::vec2> & texCoords, glm::vec2 uv0, glm::vec2 uv1);

	unsigned int meshIndices(unsigned int * indices, unsigned int start = 0);
	unsigned int meshIndices(std::vector<unsigned int> & indices, unsigned int start = 0);

	Bezier bezierRows[4];
	Bezier bezierCols[4];

	static bool tensorPatch;

    std::vector<Bezier> bezierSubRows;
    std::vector<Bezier> bezierSubCols;

private:
	void makeSubdiv(Bezier & a1, Bezier & a2, Bezier & b1, Bezier & b2, Bezier & c1, Bezier & c2, std::vector<Bezier> & beziers, size_t subdiv);
};
