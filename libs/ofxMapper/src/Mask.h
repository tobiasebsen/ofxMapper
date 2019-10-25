#pragma once

#include "ofMain.h"

class Mask {
public:

    void setPoints(vector<glm::vec2> & points);
    vector<glm::vec3> & getPoints();

    void draw();
    void drawOutline();
    
    bool select(const glm::vec2 & p);
    
    ofParameter<string> name = { "Name:", "" };
    ofParameter<bool> enabled = { "Enabled", true };
    ofParameter<bool> edit = { "Edit", false };
    ofParameter<bool> remove = { "Remove", false };
    ofParameterGroup group = { "Mask" , name, enabled, edit, remove };

    ofParameter<bool> selected = { "Selected", false };

private:
    ofPolyline poly;
    ofMesh mesh;
};

typedef shared_ptr<Mask> MaskPtr;
