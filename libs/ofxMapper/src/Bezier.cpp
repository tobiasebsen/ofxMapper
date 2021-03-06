#include "Bezier.h"
#include <glm/gtx/fast_square_root.hpp>

void Bezier::set(const glm::vec2 & a, const glm::vec2 & ac, const glm::vec2 & bc, const glm::vec2 & b, size_t resolution) {
    this->a = a;
    this->b = b;
    this->ac = ac;
	this->bc = bc;
    setResolution(resolution);
}

void Bezier::setResolution(size_t resolution) {

    size_t n = resolution+2;
    vertices.resize(n);
    distances.resize(n);

    float x0 = a.x;
    float y0 = a.y;
    
    // polynomial coefficients
    float cx = 3.0f * (ac.x - a.x);
    float bx = 3.0f * (bc.x - ac.x) - cx;
    float ax = b.x - x0 - cx - bx;
    
    float cy = 3.0f * (ac.y - a.y);
    float by = 3.0f * (bc.y - ac.y) - cy;
    float ay = b.y - y0 - cy - by;
    
    float t, t2, t3;
    float x, y;
    float d = 0;
    
    vertices[0] = glm::vec2(x0, y0);
    distances[0] = d;
    length = 0;

	glm::vec2 * vdata = vertices.data();
	glm::vec2 v1 = vdata[0];
	glm::vec2 v2;
	vdata++;

	float * ddata = distances.data();
	ddata++;
    
    for (size_t i = 1; i < n; i++){
        t = ((float)i / (float)(n-1));
        t2 = t * t;
        t3 = t2 * t;
        x = (ax * t3) + (bx * t2) + (cx * t) + x0;
        y = (ay * t3) + (by * t2) + (cy * t) + y0;
		v2 = glm::vec2(x, y);
		vdata[0] = v2;

		d = glm::fastDistance(v1, v2);
        ddata[0] = d;
        length += d;
 
		v1 = v2;
		vdata++;
		ddata++;
    }
}

/*void Bezier::setStart(const glm::vec2 & a) {
	this->a = a;
	setResolution(vertices.size() - 2);
}

void Bezier::moveStart(const glm::vec2 & delta) {
	this->a += delta;
	setResolution(vertices.size() - 2);
}

void Bezier::setEnd(const glm::vec2 & a) {
	this->b = b;
	setResolution(vertices.size() - 2);
}

void Bezier::moveEnd(const glm::vec2 & delta) {
	this->b += delta;
	setResolution(vertices.size() - 2);
}*/

std::vector<Bezier> Bezier::subdivide(int subdivisions) {

    std::vector<Bezier> beziers;
	size_t n = subdivisions + 1;
    float delta = 1.f/n;
    glm::vec2 a1 = a;
	glm::vec2 ac1 = ac;
	glm::vec2 bc1 = bc;
	glm::vec2 b1 = b;

    beziers.resize(n);

    for (int i=0; i<n; i++) {
        float u = 1.f / (n-i);

        // De Casteljau's Algorithm
        glm::vec2 p10 = glm::mix(a1, ac1, u);
        glm::vec2 p11 = glm::mix(ac1, bc1, u);
        glm::vec2 p12 = glm::mix(bc1, b1, u);
        glm::vec2 p20 = glm::mix(p10, p11, u);
        glm::vec2 p21 = glm::mix(p11, p12, u);
        glm::vec2 p30 = glm::mix(p20, p21, u);

        beziers[i].set(a1, p10, p20, p30);
        a1 = p30;
		ac1 = p21;
		bc1 = p12;
    }
    return beziers;
}

BezierSampler::BezierSampler(Bezier * bezier, size_t resolution) {
	this->bezier = bezier;
	step = bezier->length / (resolution + 1);
	vertexIndex = 1;
	delta = bezier->distances[vertexIndex];
	currentDistance = 0;
	vertexCount = bezier->vertices.size();
}

void BezierSampler::next() {
	float * ddata = bezier->distances.data();
	nextDistance += step;
	while (currentDistance + delta < nextDistance && vertexIndex < vertexCount) {
		currentDistance += delta;
		vertexIndex++;
		delta = ddata[vertexIndex];
	}
}

float BezierSampler::getWeight() {
	return (nextDistance - currentDistance) / delta;
}

float Bezier::getApproxLength(const glm::vec2 & a, const glm::vec2 & ac, const glm::vec2 & bc, const glm::vec2 & b) {
	float lc = glm::distance(a, b);
	float lp = glm::distance(a, ac) + glm::distance(ac, bc) + glm::distance(bc, b);
	return (2 * lc + lp) / 3;
}

glm::vec2 Bezier::getPointAtPercent(float f) {
    float to = f * length;
    float from = 0;
    size_t n = vertices.size();
    if (n == 0) return glm::vec3();
    glm::vec2 v1 = vertices[0];
    for (size_t i=1; i<n; i++) {
        glm::vec2 & v2 = vertices[i];
        float dist = glm::distance(v1, v2);
        if (from+dist >= to) {
            float amt = (to-from)/dist;
            return v1 + (v2 - v1) * amt;
        }
        from += dist;
        v1 = v2;
    }
    return vertices[n-1];
}
