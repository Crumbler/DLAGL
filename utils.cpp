#include "utils.hpp"

#include <glm/gtx/norm.hpp>
#include <chrono>
#include <random>

glm::vec2 Utils::Lerp(const glm::vec2 &a, const glm::vec2 &b, const float d)
{
    return a + Normalized(b - a) * d;
}

glm::vec2 Utils::Normalized(glm::vec2 a)
{
    constexpr float eps = 0.0001f;
    const float len = glm::length(a);
    if (len > eps)
        a /= len;

    return a;
}

glm::vec2 Utils::Step(const glm::vec2 a, const glm::vec2 b)
{
    return glm::vec2(a.x < b.x, a.y < b.y);
}

glm::vec2 Utils::Sign(glm::vec2 a)
{
    a.x = (a.x > 0.0f) - (a.x < 0.0f);
    a.y = (a.y > 0.0f) - (a.y < 0.0f);

    return a;
}

float Utils::Dot2(const glm::vec2 a)
{
    return glm::dot(a, a);
}

glm::vec2 Utils::Min(const glm::vec2 a, const glm::vec2 b)
{
    return glm::vec2(Min(a.x, b.x), Min(a.y, b.y));
}

float Utils::Min(const float a, const float b)
{
    if (a > b)
        return b;

    return a;
}

float Utils::Random(const float lo, const float hi)
{
    static thread_local std::mt19937 gen(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(gen);
}

glm::vec2 Utils::RandInUnitSphere()
{
    glm::vec2 p;
    do
        p = glm::vec2(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
    while (glm::length2(p) > 1.0f);

    return p;
}

int Utils::ClampInt(int x, const int lo, const int hi)
{
    if (x < lo)
        x = lo;
    else
        if (x > hi)
            x = hi;

    return x;
}

float Utils::ClampFloat(float x, const float lo, const float hi)
{
    if (x < lo)
        x = lo;
    else
        if (x > hi)
            x = hi;

    return x;
}

glm::vec3 Utils::ClampVec3(glm::vec3 a, const glm::vec3 lo, const glm::vec3 hi)
{
    a.x = ClampFloat(a.x, lo.x, hi.x);
    a.y = ClampFloat(a.y, lo.y, hi.y);
    a.z = ClampFloat(a.z, lo.z, hi.z);
    return a;
}

float Utils::sdSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
{
    const glm::vec2 ba = Normalized(b - a);
    const float h = ClampFloat(glm::dot(p - a, ba), 0.0f, glm::length(b - a));
    return glm::distance(p, a + ba * h) - 0.2f;
}

glm::vec2 Utils::sdSegmentPoint(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
{
    const glm::vec2 ba = Normalized(b - a);
    const float h = ClampFloat(glm::dot(p - a, ba), 0.0f, glm::length(b - a));
    return a + ba * h;
}

float Utils::sdCircle(const glm::vec2& p, const Circle& circle)
{
    return (glm::distance(p, circle.c) - circle.r) * (1.0f - 2.0f * circle.rev);
}

float fmin(const float a, const float b)
{
    if (a < b)
        return a;

    return b;
}

float fmax(const float a, const float b)
{
    if (a > b)
        return a;

    return b;
}

glm::vec2 Utils::sdCirclePoint(const glm::vec2& p, const Circle& circle)
{
    const glm::vec2 pc = p - circle.c,
                    npc = Normalized(pc);

    if (circle.rev == 0.0f)
        return circle.c + npc * fmin(circle.r, glm::length(pc));
    else
        return circle.c + npc * fmax(circle.r, glm::length(pc));;
}

float Utils::sdBox(const glm::vec2& p, const Box& box)
{
    using glm::dot, glm::vec2, glm::length, glm::distance;

    const vec2 baseY = Normalized(box.b - box.a),
                       baseX = vec2(baseY.y, -baseY.x);

    const float len = length(box.b - box.a);

    const vec2 proj = abs(vec2(dot(p - box.a, baseX),
                          dot(p - box.a, baseY) - len * 0.5f));

    vec2 inBoxV = Step(proj, vec2(box.th, len) * 0.5f);

    float inBox = inBoxV.x * inBoxV.y,
          distOuter = distance(vec2(box.th, len) * 0.5f, proj),
          distInner = -Min(proj.x, proj.y);

    return distInner * inBox + distOuter * (1.0f - inBox);
}

glm::vec2 Utils::sdBoxPoint(const glm::vec2& p, const Box& box)
{
    using glm::dot, glm::vec2, glm::length;

    const vec2 baseY = Normalized(box.b - box.a),
               baseX = vec2(baseY.y, -baseY.x);

    const float len = length(box.b - box.a);

    vec2 proj = vec2(dot(p - box.a, baseX),
                     dot(p - box.a, baseY));

    proj.x = ClampFloat(proj.x, -box.th * 0.5f, box.th * 0.5f);
    proj.y = ClampFloat(proj.y, 0.0f, len);

    return box.a + baseX * proj.x + baseY * proj.y;
}

float Utils::sdBezier(const glm::vec2& pos, const Bezier& bezier)
{
    using glm::vec2, glm::dot, glm::vec3;

    vec2 a = bezier.b - bezier.a;
    vec2 b = bezier.a - 2.0f * bezier.b + bezier.c;
    vec2 c = a * 2.0f;
    vec2 d = bezier.a - pos;
    float kk = 1.0f / std::max(dot(b, b), 0.01f);
    float kx = kk * dot(a, b);
    float ky = kk * (2.0f * dot(a, a) + dot(d, b)) / 3.0f;
    float kz = kk * dot(d, a);
    float res = 0.0f;
    float p = ky - kx * kx;
    float p3 = p * p * p;
    float q = kx * (2.0f * kx * kx - 3.0f * ky) + kz;
    float h = q * q + 4.0f * p3;
    if(h >= 0.0f)
    {
        h = sqrt(h);
        vec2 x = (vec2(h, -h) - q) / 2.0f;
        vec2 uv = Sign(x) * pow(abs(x), vec2(1.0f / 3.0f));
        float t = ClampFloat(uv.x + uv.y - kx, 0.0f, 1.0f);
        res = Dot2(d + (c + b * t) * t);
    }
    else
    {
        float z = sqrt(-p);
        float v = acos(q / (p * z * 2.0f)) / 3.0f;
        float m = cos(v);
        float n = sin(v) * 1.732050808f;
        vec3  t = ClampVec3(vec3(m + m, -n - m, n - m) * z - kx, vec3(), vec3(1.0f));
        res = std::min(Dot2(d + (c + b * t.x) * t.x),
                       Dot2(d + (c + b * t.y) * t.y));
    }
    return sqrt(res) - 0.2f;
}

glm::vec2 Utils::sdBezierPoint(const glm::vec2& pos, const Bezier& bezier)
{
    using glm::vec2, glm::dot, glm::vec3;

    vec2 a = bezier.b - bezier.a;
    vec2 b = bezier.a - 2.0f * bezier.b + bezier.c;
    vec2 c = a * 2.0f;
    vec2 d = bezier.a - pos;
    vec2 resVec;
    float kk = 1.0f / std::max(dot(b, b), 0.01f);
    float kx = kk * dot(a, b);
    float ky = kk * (2.0f * dot(a, a) + dot(d, b)) / 3.0f;
    float kz = kk * dot(d, a);
    float p = ky - kx * kx;
    float p3 = p * p * p;
    float q = kx * (2.0f * kx * kx - 3.0f * ky) + kz;
    float h = q * q + 4.0f * p3;
    if (h >= 0.0f)
    {
        h = sqrt(h);
        vec2 x = (vec2(h, -h) - q) / 2.0f;
        vec2 uv = Sign(x) * pow(abs(x), vec2(1.0f / 3.0f));
        float t = ClampFloat(uv.x + uv.y - kx, 0.0f, 1.0f);
        resVec = pos + d + (c + b * t) * t;
    }
    else
    {
        float z = sqrt(-p);
        float v = acos(q / (p * z * 2.0f)) / 3.0f;
        float m = cos(v);
        float n = sin(v) * 1.732050808f;
        vec3  t = ClampVec3(vec3(m + m, -n - m, n - m) * z - kx, vec3(), vec3(1.0f));
        resVec = Min(d + (c + b * t.x) * t.x,
                     d + (c + b * t.y) * t.y);

        const bool xSmaller = Dot2(d + (c + b * t.x) * t.x) < Dot2(d + (c + b * t.y) * t.y);
        const float newT = (xSmaller ? t.x : t.y);

        resVec = pos + d + (c + b * newT) * newT;
    }

    return resVec;
}

