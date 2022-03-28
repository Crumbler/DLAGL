#include "RawModel.hpp"

#include <iostream>

RawModel::RawModel(GLuint vaoID, GLuint vertexCount) : vaoID(vaoID), vertexCount(vertexCount)
{

}

RawModel::RawModel() { }

GLuint RawModel::getVaoID() const
{
    return vaoID;
}

GLuint RawModel::getVertexCount() const
{
    return vertexCount;
}

void RawModel::updateVBO(int attributeNumber, const std::vector<float>& data)
{
    glBindVertexArray(vaoID);
    GLuint vboID;
    glGetVertexAttribIuiv(attributeNumber, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    //glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RawModel::updateVBODynamic(int attributeNumber, const std::vector<float>& data)
{
    glBindVertexArray(vaoID);
    GLuint vboID;
    glGetVertexAttribIuiv(attributeNumber, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
