#version 430 core

uniform sampler2D srcTexture;
uniform float filterRadius;
uniform int depth;

uniform float exposure;

in vec2 texCoord;
out vec4 fragColor;

void main() {
    vec3 a = texture(srcTexture, vec2(texCoord.x, texCoord.y)).rgb;

    vec3 p = a * 4.0;
    p += 12.f * a;
    p *= 1.f / 16.f;

    if (depth == 2) {
        p = vec3(1.0) - exp(-p * exposure);
        const float gamma = 2.2;
        fragColor = vec4(pow(p, vec3(1.0 / gamma)), 1.f);
    }
    else {
        fragColor = vec4(p, 1.f);
    }
}