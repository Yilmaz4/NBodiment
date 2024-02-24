#version 430 core

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

vec3 polar_to_cartesian(float longitude, float latitude, float radius) {
    return vec3(radius * sin(longitude) * cos(latitude), radius * sin(longitude) * sin(latitude), radius * cos(longitude));
}

float to_radians(float degrees) {
    return degrees * PI / 180.f;
}

out vec4 fragColor;

in vec3 normal;
in uint pIdx;

uniform vec3 ambientLight;
uniform vec2 lightDir;

void main() {
    Particle p = read(pIdx);
    vec3 c = vec3(1.0) * dot(normal, polar_to_cartesian(to_radians(lightDir.x), to_radians(lightDir.y), 1.f));
    fragColor = vec4(c, 1.f);
}