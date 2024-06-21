float minDist = (p.radius + q.radius);
if (sqrt(distSqr) < minDist) {
    if (collisionType == 1) {
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
            write(pidx * offset, p);
            return;
        }
    }
    else {
        if (p.mass > q.mass) {
            q.pos = p.pos + normalize(q.pos - p.pos) * minDist;
        }
        else {
            p.pos = q.pos + normalize(p.pos - q.pos) * minDist;
        }
        vec3 relVel = q.vel - p.vel;
        float dotProduct = dot(relVel, normalize(dir));
        vec3 velNormal = dotProduct * normalize(dir);
        float impulseScalar = (-2 * dot(velNormal, normalize(dir))) / (1.0 / p.mass + 1.0 / q.mass);
        p.vel -= impulseScalar / p.mass * normalize(dir);
        q.vel += impulseScalar / q.mass * normalize(dir);
        write(i * offset, q);
        break;
    }
}