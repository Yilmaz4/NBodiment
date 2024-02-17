#version 430 core

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

layout(location = 0) uniform float uTimeDelta;
layout(location = 1) uniform mat4 uMatrix;

Particle read(uint i) {
    return Particle(
        vec3(vs[i+0], vs[i+1], vs[i+2]),
        vec3(vs[i+3], vs[i+4], vs[i+5]),
        vec3(vs[i+6], vs[i+7], vs[i+8]),
        vs[i+9], vs[i+10], vs[i+11]
    );
}

void write(uint i, Particle p) {
    vs[i+0] = p.pos.x;
    vs[i+1] = p.pos.y;
    vs[i+2] = p.pos.z;
    vs[i+3] = p.vel.x;
    vs[i+4] = p.vel.y;
    vs[i+5] = p.vel.z;
    vs[i+6] = p.acc.x;
    vs[i+7] = p.acc.y;
    vs[i+8] = p.acc.z;
    vs[i+9] = p.mass;
    vs[i+10] = p.temp;
    vs[i+11] = p.density;
}

out uint idx;

void main() {
    idx = gl_VertexID * 12;
    Particle p = read(idx);
    p.vel += p.acc * uTimeDelta;
    p.pos += p.vel * uTimeDelta;
    gl_Position = uMatrix * vec4(p.pos, 1);
    write(idx, p);
}