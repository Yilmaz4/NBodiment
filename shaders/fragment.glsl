#version 430 core

#define EPSILON 0.001

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

    vec3 albedo;
    vec3 emissionColor;
    vec3 absorptionColor;
    float luminosity;
    float specularity;
    float metallicity;
    float translucency;
    float refractive_index;
    float blurriness;
};
const int offset = 27;

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
        vec3(vs[i + 18], vs[i + 19], vs[i + 20]),
        vs[i + 21], vs[i + 22], vs[i + 23], vs[i + 24], vs[i + 25], vs[i + 26]
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
    vs[i + 18] = p.absorptionColor.r;
    vs[i + 19] = p.absorptionColor.g;
    vs[i + 20] = p.absorptionColor.b;
    vs[i + 21] = p.luminosity;
    vs[i + 22] = p.specularity;
    vs[i + 23] = p.metallicity;
    vs[i + 24] = p.translucency;
    vs[i + 25] = p.refractive_index;
    vs[i + 26] = p.blurriness;
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

layout(binding = 0) uniform sampler2D previousFrame;
uniform int accumulationFrameIndex;

layout(binding = 1) uniform samplerCube skybox;

layout(binding = 2) uniform sampler2D earthDaymap;
layout(binding = 3) uniform sampler2D earthClouds;
layout(binding = 4) uniform sampler2D starTexture;
layout(binding = 5) uniform sampler2D temperatureRamp;

uniform vec2 screenSize;
uniform float uTime;
uniform float timeDelta;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;
uniform vec3 cameraPos;
uniform vec3 ambientLight;
uniform int spp;
uniform bool globalIllumination;
uniform bool shadows;

out vec4 fragColor;

float random(inout uint state) {
    state = (state + accumulationFrameIndex * 100) * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    result = (result >> 22) ^ result;
    return float(result) / 4294967295.f;
}

float randomND(inout uint seed) {
    float theta = M_2PI * random(seed);
    float rho = sqrt(-2.f * log(random(seed)));
    return rho * cos(theta);
}

vec3 randomDirection(inout uint seed, in vec3 normal) {
    vec3 dir = normalize(vec3(randomND(seed), randomND(seed), randomND(seed)));
    return normalize(dir);
}

vec2 intersect(in vec3 origin, in vec3 dir, in vec3 pos, in float radius) {
    vec3 org = origin - pos;
    float a = dot(dir, dir);
    float b = 2.f * dot(org, dir);
    float c = dot(org, org) - radius * radius;

    float d = b * b - 4.f * a * c;
    if (d <= 0.f) return vec2(-1.f, -1.f);
    d = sqrt(d);
    return vec2(-b - d, -b + d) / (2.f * a);
}

int index(in vec3 origin, in vec3 direction, out float mt, out bool from_inside) {
    int pidx = -1;
    mt = 1.f / 0.f;
    for (int i = 0; i < numParticles * offset; i += offset) {
        Particle p = read(i);
        if (p.radius == 0) continue;
        vec2 ts = intersect(origin, direction, p.pos, p.radius);
        float t = (ts.x < 0.f ? ts.y : ts.x);
        if (ts.x >= 0 && mt > t) {
            pidx = i;
            mt = t;
            from_inside = (t == ts.y);
        }
    }
    return pidx;
}

vec3 trace(in vec3 origin, in vec3 direction, in int ridx) {
    vec3 radiance = vec3(0.f);
    vec3 throughput = vec3(1.f);
    uint seed = uint(gl_FragCoord.y + screenSize.x * gl_FragCoord.x) * ridx;

    Particle p;
    for (int depth = 0; depth < 10; depth++) {
        float mt;
        bool from_inside = false;
        int pidx = index(origin, direction, mt, from_inside);
        if (pidx == -1) {
            if (depth == 0) return vec3(-1.f);
            radiance += p.albedo * texture(skybox, direction).rgb;
            break;
        }
        p = read(pidx);
        vec3 hit = origin + direction * mt;
        origin = hit;
        vec3 normal = normalize(hit - p.pos);

        if (from_inside) {
            normal *= -1.0;
            throughput *= exp(-p.absorptionColor * mt);
        }

        if (false) { // uv texture mapping
            float u = (atan2(-normal.z, normal.x) + M_PI) / (2 * M_PI);
            float v = acos(normal.y) / M_PI;
            p.albedo = texture2D(starTexture, vec2(u, v)).rgb;
        }

        bool isRefractive = false;
        if (p.metallicity > 0.0) {
            float n1 = from_inside ? p.refractive_index : 1.f;
            float n2 = from_inside ? 1.f : p.refractive_index;
            float r = pow((n1 - n2) / (n1 + n2), 2);
            p.metallicity = mix(p.metallicity, 1.0, r + (1.0 - r) * pow(1.0 - dot(-direction, normal), 5));
        }

        vec3 diffuseRay = normalize(normal + randomDirection(seed, normal));
        float rayProbability;

        float raySelectRoll = random(seed);
        if (p.metallicity > raySelectRoll) {
            vec3 reflectionRayDir = reflect(direction, normal);
            reflectionRayDir = normalize(mix(reflectionRayDir, diffuseRay, pow(1.f - p.specularity, 2)));
            direction = reflectionRayDir;
            rayProbability = p.metallicity;
        }
        else if (p.metallicity + p.translucency > raySelectRoll) {
            vec3 refractionRayDir = refract(direction, normal, from_inside ? (p.refractive_index / 1.f) : (1.f / p.refractive_index));
            refractionRayDir = normalize(mix(refractionRayDir, normalize(normal + randomDirection(seed, -normal)), pow(p.blurriness, 2)));
            direction = refractionRayDir;
            rayProbability = p.translucency;
            isRefractive = true;
        }
        else {
            direction = diffuseRay;
            rayProbability = 1.0 - p.metallicity - p.translucency;
        }

        origin = hit + direction * EPSILON;
        rayProbability = max(rayProbability, EPSILON);

        radiance += p.emissionColor * p.luminosity * throughput;
        if (!isRefractive) {
            throughput *= p.albedo;
        }
        throughput /= rayProbability;

        float p = max(throughput.x, max(throughput.y, throughput.z));
        if (random(seed) > p) break;
        throughput /= p;
    }
    return radiance;
}

vec3 temperature_to_color(float t, float radius) { // unused
    return texture2D(temperatureRamp, vec2(0.5f, clamp(t / 25000.f, 0.f, 1.f))).rgb;
}

void main() {
    vec2 coord = (gl_FragCoord.xy + gl_SamplePosition) / screenSize;
    vec3 direction = vec3(invViewMatrix * vec4(normalize(vec3(invProjMatrix * vec4(2.f * coord - 1.f, 1.f, 1.f))), 0));
    vec3 origin = cameraPos;

    if (globalIllumination) {
        vec3 irradiance = vec3(0.f);
        for (int i = 1; i < spp + 1; i++) {
            vec3 radiance = trace(origin, direction, i);
            if (radiance.x == -1.f) {
                fragColor = texture(skybox, direction);
                return;
            }
            irradiance += radiance;
        }
        if (accumulationFrameIndex == 0) {
            fragColor = vec4(irradiance / float(spp), 1.f);
        }
        else {
            vec3 pixel = texture(previousFrame, gl_FragCoord.xy / screenSize).rgb;
            fragColor = vec4(mix(pixel, irradiance / float(spp), 1.f / accumulationFrameIndex), 1.f);
        }
    }
    else {
        float mt = 1.f / 0.f;
        int pidx = -1;
        int lightSources[100]; // max number of lights in scene, adjust as needed
        int lidx = 0;
        for (int i = 0; i < numParticles * offset; i += offset) {
            Particle p = read(i);
            if (p.radius == 0) continue;
            if (p.luminosity > 0.f) lightSources[lidx++] = i;
            vec2 ts = intersect(origin, direction, p.pos, p.radius);
            float t = (ts.x < 0.f ? ts.y : ts.x);
            if (t >= 0 && mt > t) {
                pidx = i;
                mt = t;
            }
        }
        if (pidx == -1) {
            fragColor = texture(skybox, direction);
            return;
        }
        Particle p = read(pidx);
        vec3 hit = origin + direction * mt;
        vec3 normal = normalize(hit - p.pos);

        if (false) { // uv texture mapping
            float u = (atan2(-normal.z, normal.x) + M_PI) / (2 * M_PI);
            float v = acos(normal.y) / M_PI;
            p.albedo = texture2D(starTexture, vec2(u, v)).rgb;
        }

        vec3 irradiance = vec3(0.f);
        for (int i = 0; i < lidx; i++) {
            Particle q = read(lightSources[i]);
            float mt = 0.f;
            bool from_inside;
            int obstacle = index(hit, q.pos - hit, mt, from_inside);
            if (shadows && obstacle != lightSources[i] && obstacle != pidx) continue;
            float angle = dot(normal, normalize(q.pos - p.pos)); 
            vec3 c = q.emissionColor * q.luminosity * angle * p.albedo * q.radius / pow(distance(q.pos, p.pos), 2);
            if (angle > 0.f) irradiance += c;
        }
        vec3 color = irradiance + p.emissionColor * p.luminosity + p.albedo * ambientLight;
        fragColor = vec4(color, 1.f);
    }
}