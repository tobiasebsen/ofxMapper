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
uniform float black;    // Black level. Value added to the non-overlapping part to compensate for projector black level. Default 0
uniform vec3 gain;      // Color correction gain. Applied to the entire image to compensate for projector color bias. Default (1,1,1)

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

	// Apply color correction gain
	sample.rgb = sample.rgb * gain;

	// Apply blend function brightness
	sample.rgb = sample.rgb * pow(f, 1.0/gamma);

	// Apply black level
	//sample = f * black + sample * (1.0 - black);

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

ofShader SoftEdge::shader;

string SoftEdge::getShaderSource() {
	return softEdgeFrag;
}

void SoftEdge::init() {
	//ofSetLogLevel(OF_LOG_VERBOSE);
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragHeader + softEdgeFrag + softEdgeMain);
	shader.linkProgram();
	//ofSetLogLevel(OF_LOG_NOTICE);
}

void SoftEdge::begin() {
	if (!shader.isLoaded())
		init();
	shader.begin();
}

void SoftEdge::end() {
	shader.end();
}

void SoftEdge::setUniforms(ofShader & shader, const ofRectangle * inputRect) {
	shader.setUniform2f("pos", inputRect->position);
	shader.setUniform2f("size", inputRect->width, inputRect->height);
	shader.setUniform1f("edgeTop", edgeTop);
	shader.setUniform1f("edgeBottom", edgeBottom);
	shader.setUniform1f("edgeLeft", edgeLeft);
	shader.setUniform1f("edgeRight", edgeRight);
	shader.setUniform1f("p", power);
	shader.setUniform1f("a", luminance);
	shader.setUniform1f("gamma", gamma);
	shader.setUniform1f("black", 0);
	shader.setUniform3f("gain", 1.f, 1.0f, 1.f);
}

void SoftEdge::setUniforms(const ofRectangle * inputRect) {
	setUniforms(shader, inputRect);
}