#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle {
    vec3 pos;
    vec3 vel;
    vec3 acc;
    float mass;
    float temp;
    float radius;

    float axial_tilt;
    float rotational_period;
    float yaw;

    vec3 albedo;
    vec3 emissionColor;
    vec3 absorptionColor;
    float luminosity;
    float specularity;
    float metallicity;
    float translucency;
    float refractive_index;
    float blurriness;

    int textureid;
    int normmapid;
    int specmapid;
};
const int offset = 33;

layout(std430, binding = 0) volatile buffer vBuffer {
    float vs[];
};

Particle read(in int i) {
    return Particle(
        vec3(vs[i + 0], vs[i + 1], vs[i + 2]),
        vec3(vs[i + 3], vs[i + 4], vs[i + 5]),
        vec3(vs[i + 6], vs[i + 7], vs[i + 8]),
        vs[i + 9], vs[i + 10], vs[i + 11],
        vs[i + 12], vs[i + 13], vs[i + 14],
        vec3(vs[i + 15], vs[i + 16], vs[i + 17]),
        vec3(vs[i + 18], vs[i + 19], vs[i + 20]),
        vec3(vs[i + 21], vs[i + 22], vs[i + 23]),
        vs[i + 24], vs[i + 25], vs[i + 26], vs[i + 27], vs[i + 28], vs[i + 29],
        floatBitsToInt(vs[i + 30]), floatBitsToInt(vs[i + 31]), floatBitsToInt(vs[i + 32])
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
    vs[i + 12] = p.axial_tilt;
    vs[i + 13] = p.rotational_period;
    vs[i + 14] = p.yaw;
    vs[i + 15] = p.albedo.r;
    vs[i + 16] = p.albedo.g;
    vs[i + 17] = p.albedo.b;
    vs[i + 18] = p.emissionColor.r;
    vs[i + 19] = p.emissionColor.g;
    vs[i + 20] = p.emissionColor.b;
    vs[i + 21] = p.absorptionColor.r;
    vs[i + 22] = p.absorptionColor.g;
    vs[i + 23] = p.absorptionColor.b;
    vs[i + 24] = p.luminosity;
    vs[i + 25] = p.specularity;
    vs[i + 26] = p.metallicity;
    vs[i + 27] = p.translucency;
    vs[i + 28] = p.refractive_index;
    vs[i + 29] = p.blurriness;
    vs[i + 30] = intBitsToFloat(p.textureid);
    vs[i + 31] = intBitsToFloat(p.normmapid);
    vs[i + 32] = intBitsToFloat(p.specmapid);
}

uniform float timeDelta;
uniform int collisionType; // 0: elastic, 1: inelastic, 2: none
uniform int mutualAttraction;
uniform int strongestAttractor = 0;

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

vec3 attract(Particle p, int pidx, int qidx) {
    Particle q = read(qidx * offset);
    if (q.mass < 1e-10) return vec3(0.f);
    vec3 dir = q.pos - p.pos;
    float distSqr = dot(dir, dir);

    %s // collision code

    vec3 forceDir = normalize(dir);
    float forceMagnitude = G * p.mass * q.mass / distSqr;
    vec3 force = forceDir * forceMagnitude;
    return force / p.mass;
}

void main() {
    if (timeDelta == 0) return;
    int pidx = int(gl_GlobalInvocationID.x);
    Particle p = read(pidx * offset);
    if (p.mass == 0) return;
    vec3 totalAcc = vec3(0.0);

    if (mutualAttraction == 1) {
        for (int i = 0; i < gl_NumWorkGroups.x; ++i) {
            vec3 acc = attract(p, pidx, i);
            if (isnan(acc.x)) break;
            if (isnan(acc.y)) return;
            if (i != pidx) totalAcc += acc;
        }
    }
    else if (strongestAttractor != pidx) {
        totalAcc = attract(p, pidx, strongestAttractor);
    }

    if (p.rotational_period != 0.f) {
        p.yaw += 360.f * timeDelta / p.rotational_period;
        p.yaw = mod(p.yaw, 360.f);
    }
    p.acc = totalAcc;
    p.vel += p.acc * timeDelta;
    p.pos += p.vel * timeDelta;

    write(int(gl_GlobalInvocationID.x) * offset, p);
}
