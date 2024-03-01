#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle {
    vec3 pos;
    vec3 vel;
    vec3 acc;
    float mass;
    float temp;
    float radius;

    vec3 albedo;
    vec3 emissionColor;
    float emissionStrength;
    float metallicity;
    float roughness;
};
const int offset = 21;

layout(std430, binding = 0) volatile buffer vBuffer {
    float vs[];
};
uniform int numParticles;

Particle read(in int i) {
    return Particle(
        vec3(vs[i + 0], vs[i + 1], vs[i + 2]),
        vec3(vs[i + 3], vs[i + 4], vs[i + 5]),
        vec3(vs[i + 6], vs[i + 7], vs[i + 8]),
        vs[i + 9], vs[i + 10], vs[i + 11],
        vec3(vs[i + 12], vs[i + 13], vs[i + 14]),
        vec3(vs[i + 15], vs[i + 16], vs[i + 17]),
        vs[i + 18], vs[i + 19], vs[i + 20]
    );
}

void write(in int i, in Particle p) {
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
    vs[i + 12] = p.albedo.r;
    vs[i + 13] = p.albedo.g;
    vs[i + 14] = p.albedo.b;
    vs[i + 15] = p.emissionColor.r;
    vs[i + 16] = p.emissionColor.g;
    vs[i + 17] = p.emissionColor.b;
    vs[i + 18] = p.emissionStrength;
    vs[i + 19] = p.metallicity;
    vs[i + 20] = p.roughness;
}

uniform float uTimeDelta;
uniform bool collisionType;

const float G = 6.67430e-11;
const float PI = 3.14159265;

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
    Particle p = read(int(gl_GlobalInvocationID.x) * offset);
    if (p.mass == 0) return;
    vec3 totalAcc = vec3(0.0);
    for (int i = 0; i < gl_NumWorkGroups.x; ++i) {
        if (i != int(gl_GlobalInvocationID.x)) {
            Particle q = read(i * offset);
            if (q.mass == 0) continue;
            vec3 dir = q.pos - p.pos;
            float distSqr = dot(dir, dir);
            float minDistSqr = (p.radius + q.radius) * (p.radius + q.radius);
            if (distSqr <= minDistSqr) {
                if (!collisionType) { // perfectly inelastic collision, merge masses
                    if (p.mass > q.mass) {
                        float density = p.mass / (4.f * PI * pow(p.radius, 3) / 3.f);
                        p.mass += q.mass;
                        p.radius = cbrt((3.f * (p.mass / density)) / (4.f * PI));
                        q.mass = 0;
                        q.radius = 0;
                        write(i * offset, q);
                    }
                    else {
                        float density = q.mass / (4.f * PI * pow(q.radius, 3) / 3.f);
                        q.mass += p.mass;
                        q.radius = cbrt((3.f * (q.mass / density)) / (4.f * PI));
                        p.mass = 0;
                        p.radius = 0;
                        write(i * offset, q);
                        write(int(gl_GlobalInvocationID.x) * offset, p);
                        return;
                    }
                }
                else { // elastic collision, bounce
                    vec3 relVel = q.vel - p.vel;
                    float dotProduct = dot(relVel, normalize(dir));
                    vec3 velNormal = dotProduct * normalize(dir);
                    float impulseScalar = (-2 * dot(velNormal, normalize(dir))) / (1.0 / p.mass + 1.0 / q.mass);
                    p.vel -= impulseScalar / p.mass * normalize(dir);
                    q.vel += impulseScalar / q.mass * normalize(dir);
                    write(i * offset, q);
                }
            }
            else {
                vec3 forceDir = normalize(dir);
                float forceMagnitude = G * p.mass * q.mass / distSqr;
                vec3 force = forceDir * forceMagnitude;
                totalAcc += force / p.mass;
            }
        }
    }

    p.acc = totalAcc;
    p.vel += p.acc * uTimeDelta;
    p.pos += p.vel * uTimeDelta;

    write(int(gl_GlobalInvocationID.x) * offset, p);
}