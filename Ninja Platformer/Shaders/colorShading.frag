#version 130
// The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;

//T his is the 3 component float vector that gets outputted to the screen for each pixel.
out vec4 color;

uniform sampler2D mySampler;

void main() {
    color = fragmentColor;
}