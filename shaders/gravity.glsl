#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle {
    vec3 pos;
    vec3 vel;
    vec3 acc;
    float mass;
    float temp;
    float radius;
};

layout(std430, binding = 0) volatile buffer vBuffer {
    float vs[];
};

layout(location = 0) uniform float uTimeDelta;

const float G = 6.67430e-11;
const float PI = 3.14159265;

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
    vs[i + 11] = p.radius;
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

void main() {
    if (uTimeDelta == 0) return;
    Particle p = read(gl_GlobalInvocationID.x * 12);
    if (p.mass == 0) return;
    vec3 totalAcc = vec3(0.0);
    for (int i = 0; i < gl_NumWorkGroups.x; ++i) {
        if (i != int(gl_GlobalInvocationID.x)) {
            Particle q = read(uint(i) * 12);
            if (q.mass == 0) continue;
            vec3 dir = q.pos - p.pos;
            float distSqr = dot(dir, dir);
            float minDistSqr = (p.radius + q.radius) * (p.radius + q.radius);
            if (distSqr <= minDistSqr) {
                if (p.mass > q.mass) {
                    float density = p.mass / (4.f * PI * pow(p.radius, 3) / 3.f);
                    p.mass += q.mass;
                    p.radius = cbrt((3.f * (p.mass / density)) / (4.f * PI));
                    q.mass = 0;
                    q.radius = 0;
                    write(uint(i) * 12, q);
                }
                else {
                    float density = q.mass / (4.f * PI * pow(q.radius, 3) / 3.f);
                    q.mass += p.mass;
                    q.radius = cbrt((3.f * (q.mass / density)) / (4.f * PI));
                    p.mass = 0;
                    p.radius = 0;
                    write(uint(i) * 12, q);
                    write(gl_GlobalInvocationID.x * 12, p);
                    return;
                }
            }
            vec3 forceDir = normalize(dir);
            float forceMagnitude = G * p.mass * q.mass / distSqr;
            vec3 force = forceDir * forceMagnitude;
            totalAcc += force / p.mass;
        }
    }

    p.acc = totalAcc;
    p.vel += p.acc * uTimeDelta;
    p.pos += p.vel * uTimeDelta;

    write(gl_GlobalInvocationID.x * 12, p);
}