#version 130
// The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

uniform vec2 lightPosition;
uniform vec2 dir;
out vec4 color;

void main() {
	vec2 lightDir = normalize(lightPosition - (gl_FragCoord.xy));
	float distance = length(fragmentUV);
	float diff = dot(lightDir, dir);

	float alpha = fragmentColor.a;
	if (diff < -0.99) {
		alpha = alpha * pow(0.01, distance - 0.01);
	}
	else {
		alpha = 0.0;
	}

    color = vec4(fragmentColor.rgb, alpha);
}