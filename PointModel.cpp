#include "PointModel.hpp"

PointModel::PointModel(GLuint vaoID, GLuint vertexCount, GLuint vbo0, GLuint vbo1)
    : RawModel(vaoID, vertexCount), vbo0(vbo0), vbo1(vbo1), lastInd(0) {}

PointModel::PointModel() {}

void PointModel::updatePoints(const DLAModel& dlaModel)
{
    const auto& points = dlaModel.GetPoints();
    const auto& distances = dlaModel.GetDistances();

    glBindVertexArray(vaoID);

    glBindBuffer(GL_ARRAY_BUFFER, vbo0);
    glBufferSubData(GL_ARRAY_BUFFER, lastInd * sizeof(glm::vec2), (points.size() - lastInd) * sizeof(glm::vec2), &points[lastInd]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferSubData(GL_ARRAY_BUFFER, lastInd * sizeof(int), (points.size() - lastInd) * sizeof(int), &distances[lastInd]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    vertexCount = points.size();
    lastInd = points.size();
}

void PointModel::resetPoints()
{
    lastInd = vertexCount = 0;
}
