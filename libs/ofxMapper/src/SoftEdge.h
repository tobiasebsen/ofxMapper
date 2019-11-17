#pragma once

#include "ofMain.h"

class SoftEdge {
public:
	static string getShaderSource();

	void setUniforms(const ofShader & shader, const ofRectangle & inputRect);
    void setUniforms(const ofShader & shader);

	ofParameter<float> edgeLeft = { "Left", 0, 0, 1 };
	ofParameter<float> edgeRight = { "Right", 0, 0, 1 };
	ofParameter<float> edgeTop = { "Top", 0, 0, 1 };
	ofParameter<float> edgeBottom = { "Bottom", 0, 0, 1 };

	ofParameter<float> gamma = { "Gamma", 1, 1, 4 };
	ofParameter<float> luminance = { "Luminance", 0.5, 0, 1 };
	ofParameter<float> power = { "Power", 2, 0.1, 7 };
	ofParameterGroup group = {"Soft Edge", gamma, luminance, power};

};

typedef shared_ptr<SoftEdge> SoftEdgePtr;
