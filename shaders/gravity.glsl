#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

const float G = 6.67430e-11;

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

void main() {
    Particle p = read(gl_GlobalInvocationID.x * 12);
    uint numParticles = gl_NumWorkGroups.x;

    vec3 totalAcc = vec3(0.0);
    for (int i = 0; i < numParticles; ++i) {
        if (i != int(gl_GlobalInvocationID.x)) {
            Particle other = read(uint(i) * 12);
            vec3 dir = other.pos - p.pos;
            float distSqr = dot(dir, dir);
            vec3 forceDir = normalize(dir);
            float forceMagnitude = G * p.mass * other.mass / distSqr;
            vec3 force = forceDir * forceMagnitude;
            totalAcc += force / p.mass;
        }
    }

    p.acc = totalAcc;


    write(gl_GlobalInvocationID.x * 12, p);
}