#pragma once

#include "RawModel.hpp"
#include "DLAModel.hpp"

class PointModel : public RawModel
{
public:
    PointModel(GLuint vaoID, GLuint vertexCount, GLuint vbo0, GLuint vbo1);
    PointModel();

    void updatePoints(const DLAModel& dlaModel);
    void resetPoints();

    GLuint vbo0, vbo1;
    int lastInd;
};
