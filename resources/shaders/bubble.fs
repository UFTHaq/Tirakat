#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform float radius;
uniform float power;

// Output fragment color
out vec4 finalcolor;

void main() {
    float r = 0.135;
    vec2 p = fragTexCoord - 0.5;
    if (length(p) <= 0.5) {
        float s = length(p) - r;
        if (s <= 0) {
            // finalcolor = fragColor*1.25;
        } else {
            float t = 1 - s / (0.5 - r);
            finalcolor = vec4(fragColor.xyz,t*t);
        }
    } else {
        finalcolor = vec4(0);
    }

}