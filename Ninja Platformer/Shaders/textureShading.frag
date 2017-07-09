#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D mySampler;
uniform bool flashLightOn;
uniform vec2 flashLightPosition;
uniform vec2 flashLightDirection;
uniform vec4 flashLightColor;

void main() {
	vec2 lightDir = normalize(flashLightPosition - (gl_FragCoord.xy));
	float distance = length(fragmentUV);
	float diff = dot(lightDir, flashLightDirection);
    
    vec4 textureColor = texture(mySampler, fragmentUV);

	if (diff < -0.99 && flashLightOn) {
		float alpha = 1.0;
		if (fragmentColor.a > 0.0) {
			alpha = fragmentColor.a;
		}

		color = vec4(fragmentColor.rgb, alpha * flashLightColor.a) * textureColor;
	}
	else {
		color = fragmentColor * textureColor;
	}
}