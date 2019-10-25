#pragma once

#include "ofMain.h"

class SoftEdge {
public:
	static string getShaderSource();
	static void init();

	void begin();
	void end();

	void setUniforms(ofShader & shader, const ofRectangle * inputRect);
	void setUniforms(const ofRectangle * inputRect);

	ofParameter<float> edgeLeft = { "Left", 0, 0, 1 };
	ofParameter<float> edgeRight = { "Right", 0, 0, 1 };
	ofParameter<float> edgeTop = { "Top", 0, 0, 1 };
	ofParameter<float> edgeBottom = { "Bottom", 0, 0, 1 };
	ofParameter<float> gamma = { "Gamma", 1, 1, 4 };
	ofParameter<float> luminance = { "Luminance", 0.5, 0, 1 };
	ofParameter<float> power = { "Power", 2, 0.1, 7 };

	static ofShader shader;
private:
};

typedef shared_ptr<SoftEdge> SoftEdgePtr;
