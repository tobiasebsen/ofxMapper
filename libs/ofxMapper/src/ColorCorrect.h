#pragma once

#include "ofMain.h"

class ColorCorrect {
public:

    static string getShaderSource();

    void setUniforms(const ofShader & shader);
    void setUniformsZero(const ofShader & shader);

    ofParameter<float> brightness = { "Brightness", 0, -1, 1 };
    ofParameter<float> contrast = { "Contrast", 0, -1, 1 };
    ofParameter<float> gainRed = { "Red", 0, -1, 1 };
    ofParameter<float> gainGreen = { "Green", 0, -1, 1 };
    ofParameter<float> gainBlue = { "Blue", 0, -1, 1 };
    ofParameterGroup group = { "Color correction", brightness, contrast, gainRed, gainGreen, gainBlue };
};
