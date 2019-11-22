#pragma once

#include "ofMain.h"
#include "Element.h"
#include "DragHandle.h"

namespace ofxMapper {

	class Mask : public Element, public HasHandlesT<DragHandle> {
	public:
		Mask();

		void setScreenRect(const ofRectangle & rect);
		void setPoints(vector<glm::vec2> & points);

		void update();
		void updateMesh();

		virtual void draw();
		virtual void drawOutline();

		virtual glm::vec2 getCenter();

		virtual bool select(const glm::vec2 & p);
		virtual void move(const glm::vec2 & delta);

		bool removeHandleSelected();
		void moveHandle(DragHandle & handle, const glm::vec2 & delta);
		void notifyHandles();

		using HasHandlesT<DragHandle>::moveHandle;

		ofParameter<bool> closed = { "Closed", true };
		ofParameter<bool> inverted = { "Inverted", false };
		ofParameterGroup group = { "Mask" , name, enabled, editEnabled, closed, inverted, remove };

		//ofParameter<bool> selected = { "Selected", false };

		void closedChanged(bool&);
		void invertedChanged(bool&);

	private:
		ofRectangle screenRect;
		vector<ofPolyline> poly;
		ofMesh mesh;
	};

	typedef shared_ptr<Mask> MaskPtr;
}
