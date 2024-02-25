#version 430 core

const float PI = 3.14159265359;
const float HALF_PI = PI / 2.0;

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

Particle read(int i) {
    return Particle(
        vec3(vs[i + 0], vs[i + 1], vs[i + 2]),
        vec3(vs[i + 3], vs[i + 4], vs[i + 5]),
        vec3(vs[i + 6], vs[i + 7], vs[i + 8]),
        vs[i + 9], vs[i + 10], vs[i + 11]
    );
}

void write(int i, Particle p) {
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

vec3 polar_to_cartesian(float longitude, float latitude, float radius) {
    return vec3(radius * sin(longitude) * cos(latitude), radius * sin(longitude) * sin(latitude), radius * cos(longitude));
}

float to_radians(float degrees) {
    return degrees * PI / 180.f;
}

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
    vec2 coord = gl_FragCoord.xy / screenSize;
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

    fragColor = vec4(vec3(1.f) * ambientLight * dot(polar_to_cartesian(to_radians(60), to_radians(60), 1.f), normal), 1.f);
}