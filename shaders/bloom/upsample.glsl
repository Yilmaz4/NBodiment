#version 430 core

uniform sampler2D srcTexture;
uniform int depth;
uniform float exposure;

in vec2 texCoord;
out vec4 fragColor;

void main() {
    vec2 srcTexSize = textureSize(srcTexture, 0);
    vec2 pixelSize = 1.0 / srcTexSize;

    vec2 coord00 = floor(texCoord * srcTexSize) / srcTexSize;
    vec2 coord10 = coord00 + vec2(pixelSize.x, 0.0);
    vec2 coord01 = coord00 + vec2(0.0, pixelSize.y);
    vec2 coord11 = coord00 + pixelSize;

    vec2 f = fract(texCoord * srcTexSize);

    vec3 color00 = texture(srcTexture, coord00).rgb;
    vec3 color10 = texture(srcTexture, coord10).rgb;
    vec3 color01 = texture(srcTexture, coord01).rgb;
    vec3 color11 = texture(srcTexture, coord11).rgb;

    vec3 color0 = mix(color00, color10, f.x);
    vec3 color1 = mix(color01, color11, f.x);
    vec3 interpolatedColor = mix(color0, color1, f.y);

    if (depth == 2) {
        interpolatedColor = vec3(1.0) - exp(-interpolatedColor * exposure);
        const float gamma = 2.2;
        fragColor = vec4(pow(interpolatedColor, vec3(1.0 / gamma)), 1.0);
    }
    else {
        fragColor = vec4(interpolatedColor, 1.0);
    }
}