#version 430 core

const float M_PI = 3.14159265359;
const float M_PI2 = M_PI / 2.f;
const float M_2PI = M_PI * 2.f;

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
uniform int numParticles;

Particle read(in int i) {
    return Particle(
        vec3(vs[i + 0], vs[i + 1], vs[i + 2]),
        vec3(vs[i + 3], vs[i + 4], vs[i + 5]),
        vec3(vs[i + 6], vs[i + 7], vs[i + 8]),
        vs[i + 9], vs[i + 10], vs[i + 11]
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
}

vec3 polar_to_cartesian(in float longitude, in float latitude, in float radius) {
    return vec3(radius * sin(longitude) * cos(latitude), radius * sin(longitude) * sin(latitude), radius * cos(longitude));
}

float to_radians(in float degrees) {
    return degrees * M_PI / 180.f;
}

float atan2(in float y, in float x) {
    bool s = (abs(x) > abs(y));
    return mix(M_PI / 2.0 - atan(x, y), atan(y, x), s);
}

layout(binding = 0) uniform sampler2D earthDaymap;
layout(binding = 1) uniform sampler2D earthClouds;
layout(binding = 2) uniform sampler2D starTexture;
layout(binding = 3) uniform sampler1D starColor;

uniform vec2 screenSize;
uniform float uTime;
uniform float uTimeDelta;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;
uniform vec3 cameraPos;
uniform vec3 ambientLight;

out vec4 fragColor;

void main() {
    vec2 coord = (gl_FragCoord.xy + gl_SamplePosition) / screenSize;
    vec3 direction = vec3(invViewMatrix * vec4(normalize(vec3(invProjMatrix * vec4(2.f * coord - 1.f, 1.f, 1.f))), 0));

    int closest = -1;
    float minT = 1.f / 0.f;
    for (int i = 0; i < numParticles * 12; i += 12) {
        Particle p = read(i);
        if (p.radius == 0) continue;
        vec3 origin = cameraPos - p.pos;
        float a = dot(direction, direction);
        float b = 2.f * dot(origin, direction);
        float c = dot(origin, origin) - p.radius * p.radius;

        float d = b * b - 4.f * a * c;
        if (d <= 0.f) continue;
        float t = (-b - sqrt(d)) / (2.f * a);
        if (t < minT && t >= 0) {
            minT = t;
            closest = i;
        }
    }
    if (closest < 0) {
        fragColor = vec4(0.f);
        return;
    }
    Particle p = read(closest);
    vec3 origin = cameraPos - p.pos;
    vec3 hit = origin + direction * minT;
    vec3 normal = normalize(hit);
    float dL = dot(polar_to_cartesian(to_radians(60), to_radians(60), 1.f), normal);

    float u = (atan2(-normal.z, normal.x) + M_PI) / (2 * M_PI);
    float v = acos(normal.y) / M_PI;
    
    if (p.temp > 2e+3) fragColor = texture2D(starTexture, vec2(u, v));
    else fragColor = vec4((texture2D(earthDaymap, vec2(u, v)) + texture2D(earthClouds, vec2(u, v))).rgb * dL, 1.f);
}