#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform float radius;
uniform float power;

// Output fragment color
out vec4 finalcolor;

void main() {
    // finalcolor = vec4(0,1,0,1);
    // finalcolor = fragColor;
    // float x = fragTexCoord.x;
    // float y = fragTexCoord.y;
    // vec2 coor = fragTexCoord;
    // finalcolor = vec4(coor,0,1);

    float r = 0.175;
    vec2 p = fragTexCoord - 0.5;
    if (length(p) <= 0.5) {
        float s = length(p) - r;
        if (s <= 0) {
            // float t = s / (r - 0.5);
            // finalcolor = vec4(fragColor.xyz,t);
            finalcolor = fragColor*1.15;
        } else {
            float t = 1 - s / (0.5 - r);
            finalcolor = vec4(fragColor.xyz,t*t);

            // effect gelembung
            // float t = s / (0.5 - r);
            // finalcolor = vec4(1,1,1,t*t);
        }
    } else {
        finalcolor = vec4(0);
    }

}