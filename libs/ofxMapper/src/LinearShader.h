#define STR(a) #a

#include <string>

static std::string vertSource = "#version 120\n"
STR(
    attribute vec2 c1;
    attribute vec2 c2;
    attribute vec2 c3;
    attribute vec2 c4;
    varying vec2 vpos;
    varying vec2 d1;
    varying vec2 b1;
    varying vec2 b2;
    varying vec2 b3;
    attribute vec2 t1;
    attribute vec2 t2;
    attribute vec2 t3;
    attribute vec2 t4;
    varying vec2 st1;
    varying vec2 st2;
    varying vec2 st3;
    varying vec2 st4;

    void main() {
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        vpos = gl_Vertex.xy;
        d1 = c2;
        vec2 d2 = c1;
        vec2 d3 = c3;
        vec2 d4 = c4;
        b1 = d2 - d1;
        b2 = d3 - d1;
        b3 = d1 - d2 - d3 + d4;
        st1 = t1;
        st2 = t2;
        st3 = t3;
        st4 = t4;
    }
    );

static std::string fragHeader = "#version 120\n"
STR(
	uniform sampler2DRect tex;
);

static std::string quadCoordFrag =
STR(
    varying vec2 vpos;
    varying vec2 d1;
    varying vec2 b1;
    varying vec2 b2;
    varying vec2 b3;
    varying vec2 st1;
    varying vec2 st2;
    varying vec2 st3;
    varying vec2 st4;

    float wedge2D(vec2 v, vec2 w) {
        return v.x*w.y - v.y*w.x;
    }
    
	vec2 quadCoord() {
        vec2 q = vpos - d1;
        float A = wedge2D(b2, b3);
        float B = wedge2D(b3, q) - wedge2D(b1, b2);
        float C = wedge2D(b1, q);
        
        // Solve for v
        vec2 uv;
        if (abs(A) < 0.001)
        {
            // Linear form
            uv.y = -C/B;
        }
        else
        {
            // Quadratic form. Take positive root for CCW winding with V-up
            float discrim = B*B - 4.0*A*C;
            uv.y = 0.5 * (-B + sqrt(discrim)) / A;
        }
        
        // Solve for u, using largest-magnitude component
        vec2 denom = b1 + uv.y * b3;
        if (abs(denom.x) > abs(denom.y))
            uv.x = 1.0 - (q.x - b2.x * uv.y) / denom.x;
        else
            uv.x = 1.0 - (q.y - b2.y * uv.y) / denom.y;

		return uv;
    }
);

static std::string quadCoordMain =
STR(
	void main() {

		// Get normalized quad UV
		vec2 uv = quadCoord();

		// Bilinear interpolation to texture coordinates
		vec2 texCoord = mix(mix(st1, st2, uv.x), mix(st4, st3, uv.x), uv.y);

		gl_FragColor = texture2DRect(tex, texCoord);
	}
);

static std::string quadCoordSoftEdgeMain =
STR(
void main() {

	// Get normalized quad UV
	vec2 uv = quadCoord();

	// Bilinear interpolation to texture coordinates
	vec2 texCoord = mix(mix(st1, st2, uv.x), mix(st4, st3, uv.x), uv.y);
	vec4 sample = texture2DRect(tex, texCoord);

    gl_FragColor = softEdge(sample, uv);
}
);

