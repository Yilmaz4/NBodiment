#version 430 core

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform ivec2 screenSize;
uniform float transparency;
uniform int accumulationFrameIndex;

void main() {
	vec3 pixel = texture(screenTexture, vec2(gl_FragCoord.x / screenSize.x, gl_FragCoord.y / screenSize.y)).rgb;
	if (accumulationFrameIndex > 0) {
		pixel /= accumulationFrameIndex;
	}
	fragColor = vec4(pixel, 1.f - transparency);
}