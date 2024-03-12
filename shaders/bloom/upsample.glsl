#version 430 core

uniform sampler2D srcTexture;
uniform float filterRadius;
uniform int depth;

uniform float exposure;

in vec2 texCoord;
out vec3 fragColor;

void main() {
    float x = 0;
    float y = 0;

    vec3 a = texture(srcTexture, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    vec3 b = texture(srcTexture, vec2(texCoord.x, texCoord.y + y)).rgb;
    vec3 c = texture(srcTexture, vec2(texCoord.x + x, texCoord.y + y)).rgb;

    vec3 d = texture(srcTexture, vec2(texCoord.x - x, texCoord.y)).rgb;
    vec3 e = texture(srcTexture, vec2(texCoord.x, texCoord.y)).rgb;
    vec3 f = texture(srcTexture, vec2(texCoord.x + x, texCoord.y)).rgb;

    vec3 g = texture(srcTexture, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    vec3 h = texture(srcTexture, vec2(texCoord.x, texCoord.y - y)).rgb;
    vec3 i = texture(srcTexture, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    vec3 p = e * 4.0;
    p += (b + d + f + h) * 2.0;
    p += (a + c + g + i);
    p *= 1.0 / 16.0;

    if (depth == 2) {
        p = vec3(1.0) - exp(-p * exposure);
        const float gamma = 2.2;
        fragColor = pow(p, vec3(1.0 / gamma));
    }
    else {
        fragColor = p;
    }
}