#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 256) out;

uniform float radius;

in vec3 fragColor;
out vec3 fragColor;

void main() {
    vec4 worldPosition = gl_in[0].gl_Position;
    vec4 viewPosition = viewMatrix * worldPosition;
    float distanceToCamera = length(viewPosition.xyz - cameraPosition);

    // Determine the number of segments based on distance to camera
    int numSegments = int(distanceToCamera * 0.1); // You can adjust the factor as needed

    // Clamp the number of segments to a reasonable range
    numSegments = clamp(numSegments, 4, 128);

    // Generate sphere vertices
    for (int i = 0; i <= numSegments; ++i) {
        float phi = 3.14159265359 * float(i) / float(numSegments);
        float cosPhi = cos(phi);
        float sinPhi = sin(phi);

        for (int j = 0; j <= numSegments; ++j) {
            float theta = 2.0 * 3.14159265359 * float(j) / float(numSegments);
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            vec3 normal = vec3(cosTheta * sinPhi, cosPhi, sinTheta * sinPhi);
            vec3 position = worldPosition.xyz + radius * normal;

            fragPosition = position;

            gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}