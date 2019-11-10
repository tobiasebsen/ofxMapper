#pragma once

#include "ofMain.h"

class Vertices {
public:
    Vertices(size_t w, size_t h) : width(w), height(h) {
        data = new glm::vec2[w*h];
    }
    ~Vertices() {
        delete data;
    }

    glm::vec2 * data = NULL;
    size_t width = 0;
    size_t height = 0;
};

typedef shared_ptr<Vertices> VerticesPtr;
