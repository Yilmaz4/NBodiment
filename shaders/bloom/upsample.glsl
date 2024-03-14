#version 430 core

uniform sampler2D srcTexture;
uniform float filterRadius;
uniform int depth;

uniform float exposure;

in vec2 texCoord;
out vec3 fragColor;

void main() {
    vec3 a = texture(srcTexture, vec2(texCoord.x, texCoord.y)).rgb;

    vec3 p = a * 4.0;
    p += (a + a + a + a) * 2.0;
    p += (a + a + a + a);
    p *= 1.f / 16.f;

    if (depth == 2) {
        p = vec3(1.0) - exp(-p * exposure);
        const float gamma = 2.2;
        fragColor = pow(p, vec3(1.0 / gamma));
    }
    else {
        fragColor = p;
    }
}