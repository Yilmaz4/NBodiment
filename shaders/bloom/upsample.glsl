#version 430 core

uniform sampler2D srcTexture;
uniform float filterRadius;
uniform int depth;

in vec2 texCoord;
out vec4 fragColor;

void main() {
    float x = filterRadius;
    float y = filterRadius;

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
    fragColor = vec4(p, depth / 4);
}