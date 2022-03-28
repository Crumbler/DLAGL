#pragma once

#include "glm/glm.hpp"
#include "DLABase.hpp"

namespace Utils
{
    glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, const float d);
    glm::vec2 Normalized(glm::vec2 a);
    glm::vec2 Step(const glm::vec2 a, const glm::vec2 b);
    glm::vec2 Sign(glm::vec2 a);
    float Dot2(const glm::vec2 a);
    float Min(const float a, const float b);
    glm::vec2 Min(const glm::vec2 a, const glm::vec2 b);
    float Random(const float lo = 0.0f, const float hi = 1.0f);
    glm::vec2 RandInUnitSphere();
    int ClampInt(int x, const int lo, const int hi);
    float ClampFloat(float x, const float lo, const float hi);
    glm::vec3 ClampVec3(const glm::vec3 a, const glm::vec3 lo, const glm::vec3 hi);

    float sdSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b);
    glm::vec2 sdSegmentPoint(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b);

    float sdCircle(const glm::vec2& p, const Circle& circle);
    glm::vec2 sdCirclePoint(const glm::vec2& p, const Circle& circle);

    float sdBox(const glm::vec2& p, const Box& box);
    glm::vec2 sdBoxPoint(const glm::vec2& p, const Box& box);

    float sdBezier(const glm::vec2& pos, const Bezier& bezier);
    glm::vec2 sdBezierPoint(const glm::vec2& pos, const Bezier& bezier);
}
