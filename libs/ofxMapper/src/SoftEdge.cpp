#include "SoftEdge.h"

#define STR(a) #a

static string fragHeader = "#version 120\n"
STR(
uniform sampler2DRect tex;
);

static string softEdgeFrag =
STR(
uniform vec2 pos;       // Position offset of rendered texture-subsection. Typical (0,0)
uniform vec2 size;      // Size of rendered texture or texture sub-section.
uniform float edgeLeft; // Left edge procentage. Range 0-1
uniform float edgeRight;// Right edge procentage. Range 0-1
uniform float edgeTop;  // Top edge procentage. Range 0-1
uniform float edgeBottom;// Bottom edge procentage. Range 0-1
uniform float p;        // Blend curve. Typical 1-3
uniform float a;        // Blend center brightness. Default 0.5
uniform float gamma;    // Gamma. Blend region inverse-gamma to compensate for projectors own gamma. Default 1

vec4 softEdge(vec4 sample, vec2 uv) {

	// Calculate the blend position (x)
	float x = 1.0;
	if (uv.x < edgeLeft)
		x = uv.x / edgeLeft;
	if (uv.x > 1.0 - edgeRight)
		x = (1.0 - uv.x) / edgeRight;

	// Calculate blend function
	float f = 1.0;
	if (x < 0.5)
		f = a * pow(2.0 * x, p);
	else
		f = 1.0 - (1.0 - a) * pow(2.0 * (1.0 - x), p);

	// Apply blend function brightness
	sample.rgb = sample.rgb * pow(f, gamma);

	return sample;
}
);

static string softEdgeMain =
STR(
void main() {
	vec2 texCoord = gl_TexCoord[0].st;
	vec4 sample = texture2DRect(tex, texCoord);
	vec2 uv = (texCoord - pos) / size;

	gl_FragColor = softEdge(sample, uv);
}
);

string SoftEdge::getShaderSource() {
	return softEdgeFrag;
}

void SoftEdge::setUniforms(const ofShader & shader, const ofRectangle & inputRect) {
	shader.setUniform2f("pos", inputRect.position);
	shader.setUniform2f("size", inputRect.width, inputRect.height);
	shader.setUniform1f("edgeTop", edgeTop);
	shader.setUniform1f("edgeBottom", edgeBottom);
	shader.setUniform1f("edgeLeft", edgeLeft);
	shader.setUniform1f("edgeRight", edgeRight);
	shader.setUniform1f("p", power);
	shader.setUniform1f("a", luminance);
	shader.setUniform1f("gamma", 1.f / gamma);
	shader.setUniform1f("black", 0);
	shader.setUniform3f("gain", 1.f, 1.0f, 1.f);
}
