#pragma once

#include "ofMain.h"

class DragHandle {
public:
	glm::vec2 position;
	bool selected = false;
	bool dragging = false;
};


class HasHandles {
public:
	virtual size_t getNumHandles() const = 0;
	//virtual DragHandle * getHandle(size_t) = 0;
	//virtual const DragHandle * getHandle(size_t) const = 0;
	virtual bool selectHandle(const glm::vec2 & p, float radius) = 0;
	virtual bool grabHandle(const glm::vec2 & p, float radius) = 0;
	virtual bool dragHandle(const glm::vec2 & delta) = 0;
	virtual void releaseHandle() = 0;
};

template<typename T = DragHandle>
class HasHandlesT : HasHandles {
public:
	vector<T> & getHandles() {
		return handles;
	}

	size_t getNumHandles() const {
		return handles.size();
	}

	T * getHandle(size_t handleIndex) {
		return &handles[handleIndex];
	}

	const T * getHandle(size_t handleIndex) const {
		return &handles[handleIndex];
	}

	bool selectHandle(const glm::vec2 & p, float radius) {
		bool selected = false;
		for (T & h : handles) {
			if (glm::distance(p, h.position) < radius)
				selected = true;
		}
		return selected;
	}

	bool grabHandle(const glm::vec2 & p, float radius) {
		bool grabbed = false;
		for (T & h : handles) {
			if (glm::distance(p, h.position) <= radius) {
				grabbed = true;
				h.selected = true;
				h.dragging = true;
			}
			else {
				h.selected = false;
				h.dragging = false;
			}
		}
		return grabbed;
	}

	bool dragHandle(const glm::vec2 & delta) {
		bool drag = false;
		for (T & h : handles) {
			if (h.dragging) {
				drag = true;
				moveHandle(h, delta);
			}
		}
		if (drag) {
			notifyHandles();
		}
		return drag;
	}

	virtual void moveHandle(T & handle, const glm::vec2 & delta) = 0;
	virtual void notifyHandles() {}

	void releaseHandle() {
		for (T & h : handles) {
			h.dragging = false;
		}
	}

protected:
	vector<T> handles;
};