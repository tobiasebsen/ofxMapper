#pragma once

#include <vector>
#include "glm/glm.hpp"

class BezierSampler;
template<typename T>
class BezierSamplerT;

class Bezier {
public:
    Bezier() {}
    Bezier(Bezier &&) = default;
	Bezier(const glm::vec2 & a, const glm::vec2 & ac, const glm::vec2 & bc, const glm::vec2 & b, size_t resolution = 20) {
        set(a, ac, bc, b, resolution);
	}

    void set(const glm::vec2 & a, const glm::vec2 & ac, const glm::vec2 & bc, const glm::vec2 & b, size_t resolution = 20);
    void setResolution(size_t resolution);
	size_t getResolution() {
		return vertices.size() - 2;
	}

	glm::vec2 getStart() {
		return a;
	}
	void setStart(const glm::vec2 & a);
	void moveStart(const glm::vec2 & delta);

	glm::vec2 getEnd() {
		return b;
	}
	void setEnd(const glm::vec2 & b);
	void moveEnd(const glm::vec2 & delta);

    glm::vec2 getC1() {
		return ac - a;
	}

	glm::vec2 getC2() {
		return bc - b;
	}

    std::vector<glm::vec2> & getVertices() {
		return vertices;
	}

    std::vector<Bezier> subdivide(int subdivisions);

	template<typename T>
	T getVertex(size_t vertexIndex);
    
	template<typename T>
	void getResampled(size_t resolution, T * samples) {
		BezierSamplerT<T> sampler(this, resolution);
		sampler.sample(resolution, samples);
	}

	template<typename T>
	void getResampled(size_t resolution, std::vector<T> & samples) {
		samples.resize(resolution + 2);
		getResampled(resolution, samples.data());
	}

    float getLength() {
        return length;
    }

	static float getApproxLength(const glm::vec2 & a, const glm::vec2 & ac, const glm::vec2 & bc, const glm::vec2 & b);
    
    float getDistance() {
        return glm::distance(a, b);
    }

    glm::vec2 getPointAtPercent(float f);

protected:
    glm::vec2 a;
    glm::vec2 b;
    glm::vec2 ac;
	glm::vec2 bc;
    float length;
    
    std::vector<glm::vec2> vertices;
    std::vector<float> distances;
 
	friend BezierSampler;
};

template<>
inline glm::vec2 Bezier::getVertex<glm::vec2>(size_t vertexIndex) {
	return vertices[vertexIndex];
}

template<>
inline glm::vec3 Bezier::getVertex<glm::vec3>(size_t vertexIndex) {
	return glm::vec3(vertices[vertexIndex], 0);
}

class BezierSampler {
public:
	BezierSampler(Bezier * bezier, size_t resolution);
	void next();
	float getWeight();

protected:

	Bezier * bezier;

	float currentDistance = 0;
	float nextDistance = 0;
	float step;
	float delta;

	size_t vertexCount;
	size_t vertexIndex = 1;
};

template<typename T>
class BezierSamplerT : public BezierSampler {
public:
	BezierSamplerT(Bezier * bezier, size_t resolution) : BezierSampler(bezier, resolution) {}
	T getNextSample() {
		next();
		T v1 = bezier->getVertex<T>(vertexIndex-1);
		T v2 = bezier->getVertex<T>(vertexIndex);
		return v1 + (v2 - v1) * getWeight();
	}
	void sample(size_t resolution, T * samples) {
		samples[0] = bezier->getVertex<T>(0);
		for (size_t i = 1; i <= resolution; i++) {
			samples[i] = getNextSample();
		}
		samples[resolution+1] = bezier->getVertex<T>(bezier->getResolution()+1);
	}
};
