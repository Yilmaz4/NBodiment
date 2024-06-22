#version 430 core
in vec3 aPos;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

uniform int renderer;

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
uniform int numParticles;

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

out vec3 albedo;

void main() {
    if (renderer == 0) {
        Particle p = read(gl_VertexID * offset);
        gl_Position = (projMatrix * viewMatrix) * vec4(p.pos, 1);
        albedo = p.albedo;
    } else {
        gl_Position = vec4(aPos, 1.f);
        albedo = vec3(0.f);
    }
}