#version 430 core

uniform sampler2D srcTexture;
uniform vec2 srcResolution;
uniform int depth;
uniform float threshold;

in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

void main()
{
    vec2 srcTexelSize = 1.0 / srcResolution;
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    vec3 data[13];

    data[0] = texture(srcTexture, vec2(texCoord.x - 2 * x, texCoord.y + 2 * y)).rgb;
    data[1] = texture(srcTexture, vec2(texCoord.x, texCoord.y + 2 * y)).rgb;
    data[2] = texture(srcTexture, vec2(texCoord.x + 2 * x, texCoord.y + 2 * y)).rgb;

    data[3] = texture(srcTexture, vec2(texCoord.x - 2 * x, texCoord.y)).rgb;
    data[4] = texture(srcTexture, vec2(texCoord.x, texCoord.y)).rgb;
    data[5] = texture(srcTexture, vec2(texCoord.x + 2 * x, texCoord.y)).rgb;

    data[6] = texture(srcTexture, vec2(texCoord.x - 2 * x, texCoord.y - 2 * y)).rgb;
    data[7] = texture(srcTexture, vec2(texCoord.x, texCoord.y - 2 * y)).rgb;
    data[8] = texture(srcTexture, vec2(texCoord.x + 2 * x, texCoord.y - 2 * y)).rgb;

    data[9] = texture(srcTexture, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    data[10] = texture(srcTexture, vec2(texCoord.x + x, texCoord.y + y)).rgb;
    data[11] = texture(srcTexture, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    data[12] = texture(srcTexture, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    for (int i = 0; i < 13 && depth == 0; i++) {
        data[i] -= threshold;
    }

    vec3 p = data[4] * 0.125;
    p += (data[0] + data[2] + data[6] + data[8]) * 0.03125;
    p += (data[1] + data[3] + data[5] + data[7]) * 0.0625;
    p += (data[9] + data[10] + data[11] + data[12]) * 0.125;
    fragColor = vec4(p, 0.2);
}