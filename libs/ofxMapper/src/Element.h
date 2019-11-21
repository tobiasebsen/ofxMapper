#pragma once

#include "ofMain.h"

class Element {
public:

	virtual void draw() = 0;
	virtual void drawOutline() = 0;

	virtual glm::vec2 getCenter() = 0;

	virtual bool select(const glm::vec2 & p) = 0;

	string uniqueId;
	ofParameter<string> name = { "Name:", "" };
	ofParameter<bool> enabled = { "Enabled", true };
	ofParameter<bool> editEnabled = { "Edit", false };
	ofParameter<bool> remove = { "Remove", false };
	ofParameter<bool> selected = { "Selected", false };
};

typedef shared_ptr<Element> ElementPtr;