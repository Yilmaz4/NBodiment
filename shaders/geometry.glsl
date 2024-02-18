#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 1024) out;

layout(location = 1) uniform mat4 uMatrix;
layout(location = 2) uniform vec3 cameraPos;

in uint idx[];

const float PI = 3.14159265359;
const float HALF_PI = PI / 2.0;

struct Particle {
    vec3 pos;
    vec3 vel;
    vec3 acc;
    float mass;
    float temp;
    float density;
};

layout(std430, binding = 0) volatile buffer vBuffer {
    float vs[];
};

Particle read(uint i) {
    return Particle(
        vec3(vs[i + 0], vs[i + 1], vs[i + 2]),
        vec3(vs[i + 3], vs[i + 4], vs[i + 5]),
        vec3(vs[i + 6], vs[i + 7], vs[i + 8]),
        vs[i + 9], vs[i + 10], vs[i + 11]
    );
}

void write(uint i, Particle p) {
    vs[i + 0] = p.pos.x;
    vs[i + 1] = p.pos.y;
    vs[i + 2] = p.pos.z;
    vs[i + 3] = p.vel.x;
    vs[i + 4] = p.vel.y;
    vs[i + 5] = p.vel.z;
    vs[i + 6] = p.acc.x;
    vs[i + 7] = p.acc.y;
    vs[i + 8] = p.acc.z;
    vs[i + 9] = p.mass;
    vs[i + 10] = p.temp;
    vs[i + 11] = p.density;
}

float cbrt(float x) { // https://www.shadertoy.com/view/wts3RX
    float y = sign(x) * uintBitsToFloat(floatBitsToUint(abs(x)) / 3u + 0x2a514067u);

    for (int i = 0; i < 2; i++)
        y = (2. * y + x / (y * y)) * .333333333;
    for (int i = 0; i < 2; i++) {
        float y3 = y * y * y;
        y *= (y3 + 2. * x) / (2. * y3 + x);
    }
    return y;
}

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 polar_to_cartesian(float longitude, float latitude, float radius) {
    return vec3(radius * sin(longitude) * cos(latitude), radius * sin(longitude) * sin(latitude), radius * cos(longitude));
}

void main() {
    Particle p = read(idx[0]);
    float r = cbrt((3 * (p.mass / p.density)) / (4 * PI));
    int resolution = 10;
    for (int i = 0; i <= resolution; i++) {
        for (int j = 0; j <= resolution; j++) {
            float lon = map(j, 0, 10, -PI, PI);
            gl_Position = uMatrix * vec4(p.pos + polar_to_cartesian(lon, map(i + 0, 0, 10, -HALF_PI, HALF_PI), r), 1);
            EmitVertex();
            gl_Position = uMatrix * vec4(p.pos + polar_to_cartesian(lon, map(i + 1, 0, 10, -HALF_PI, HALF_PI), r), 1);
            EmitVertex();
        }
        EndPrimitive();
    }
}