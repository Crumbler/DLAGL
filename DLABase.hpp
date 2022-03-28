#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Circle
{
    glm::vec2 c;

    float r, rev;

    Circle() : c(glm::vec2()), r(10.0f), rev(0.0f) {}
};

struct Line
{
    glm::vec2 a, b;

    Line() : a(50.0f), b(-50.0f) {}
};

struct Box
{
    glm::vec2 a, b;
    float th;

    Box() : a(-50.0f, 50.0f), b(50.0f, -50.0f), th(20.0f) {}
};

struct Bezier
{
    glm::vec2 a, b, c;

    Bezier() : a(-50.0f), b(50.0f, -50.0f), c(50.0f) {}
};

struct DLABase
{
    DLABase();

    bool IsEmpty() const;

    std::vector<glm::vec2> points;
    std::vector<Line> lines;
    std::vector<Circle> circles;
    std::vector<Box> boxes;
    std::vector<Bezier> beziers;
};
