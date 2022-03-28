#include "DLABase.hpp"


DLABase::DLABase() : points{ glm::vec2() } {}

bool DLABase::IsEmpty() const
{
    return (points.size() + lines.size() +
            circles.size() + boxes.size() +
            beziers.size()) == 0;
}
