#pragma once

#include "ofMain.h"
#include "DragHandle.h"

class Mask : public HasHandlesT<DragHandle> {
public:

    void setPoints(vector<glm::vec2> & points);

	void update();

    void draw();
    void drawOutline();
    
    bool select(const glm::vec2 & p);
  
	void moveHandle(DragHandle & handle, const glm::vec2 & delta);
	void notifyHandles();

	ofParameter<string> name = { "Name:", "" };
    ofParameter<bool> enabled = { "Enabled", true };
    ofParameter<bool> editEnabled = { "Edit", false };
    ofParameter<bool> remove = { "Remove", false };
    ofParameterGroup group = { "Mask" , name, enabled, editEnabled, remove };

    ofParameter<bool> selected = { "Selected", false };

private:
	//vector<DragHandle> handles;
    ofPolyline poly;
    ofMesh mesh;
};

typedef shared_ptr<Mask> MaskPtr;
