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
        color.r *= gainRed;
        color.g *= gainGreen;
        color.b *= gainBlue;
        color += brightness;
        color = ((color - 0.5) * (contrast + 1.0)) + 0.5;
        return color;
    }
    );

string ColorCorrect::getShaderSource() {
    return colorFrag;
}

void ColorCorrect::setUniforms(const ofShader &shader) {
    shader.setUniform1f("gainRed", gainRed / 100.f + 1.f);
    shader.setUniform1f("gainGreen", gainGreen / 100.f + 1.f);
    shader.setUniform1f("gainBlue", gainBlue / 100.f + 1.f);
    shader.setUniform1f("brightness", brightness / 100.f);
    shader.setUniform1f("contrast", contrast / 100.f);
}

void ColorCorrect::setUniformsZero(const ofShader &shader) {
    shader.setUniform1f("gainRed", 1);
    shader.setUniform1f("gainGreen", 1);
    shader.setUniform1f("gainBlue", 1);
    shader.setUniform1f("brightness", 0);
    shader.setUniform1f("contrast", 0);
}
