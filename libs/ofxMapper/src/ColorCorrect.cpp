#include "ColorCorrect.h"

#define STR(a) #a

static string colorFrag =
STR(
    uniform float gainRed;
    uniform float gainGreen;
    uniform float gainBlue;
    uniform float brightness;
    uniform float contrast;
    
    vec4 colorCorrect(vec4 color) {
        color.r *= (gainRed + 1.0);
        color.g *= (gainGreen + 1.0);
        color.b *= (gainBlue + 1.0);
        color += brightness;
        color = ((color - 0.5) * (contrast + 1.0)) + 0.5;
        return color;
    }
    );

string ColorCorrect::getShaderSource() {
    return colorFrag;
}

void ColorCorrect::setUniforms(const ofShader &shader) {
    shader.setUniform1f("gainRed", gainRed);
    shader.setUniform1f("gainGreen", gainGreen);
    shader.setUniform1f("gainBlue", gainBlue);
    shader.setUniform1f("brightness", brightness);
    shader.setUniform1f("contrast", contrast);
}

void ColorCorrect::setUniformsZero(const ofShader &shader) {
    shader.setUniform1f("gainRed", 0);
    shader.setUniform1f("gainGreen", 0);
    shader.setUniform1f("gainBlue", 0);
    shader.setUniform1f("brightness", 0);
    shader.setUniform1f("contrast", 0);
}
