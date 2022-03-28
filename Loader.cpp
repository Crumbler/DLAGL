#include "Loader.hpp"
#include <glad/glad.h>

#include <iostream>

Loader::Loader()
{

}

PointModel Loader::loadPointsEmpty()
{
    int vaoID = createVAO();
    auto v0 = storeEmptyPointPositions(0);
    auto v1 = storeEmptyHues(1);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    unbindVAO();
    return PointModel(vaoID, 0, v0, v1);
}

RawModel Loader::loadToVAO(const std::vector<float>& positions,
                           const std::vector<float>& hues)
{
    int vaoID = createVAO();
    storeDataInAttributeList(0, positions);
    storeHues(1, hues);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    unbindVAO();
    return RawModel(vaoID, hues.size());
}

RawModel Loader::loadDLABase(const std::vector<float>& positions,
                             const std::vector<float>& uvs)
{
    int vaoID = createVAO();
    storeStatic(0, positions);
    storeStatic(1, uvs);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    unbindVAO();

    return RawModel(vaoID, positions.size() / 2);
}

int Loader::createVAO()
{
    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    vaos.push_back(vaoID);
    glBindVertexArray(vaoID);
    return vaoID;
}

int Loader::storeEmptyHues(int attributeNumber)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 100 * 1024, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribIPointer(attributeNumber, 1, GL_INT, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vboID;
}

int Loader::storeEmptyPointPositions(int attributeNumber)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 200 * 1024, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(attributeNumber, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vboID;
}

void Loader::storeDataInAttributeList(int attributeNumber, const std::vector<float>& data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Loader::storeDynamic(int attributeNumber, const std::vector<float>& data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(attributeNumber, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Loader::storeStatic(int attributeNumber, const std::vector<float>& data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Loader::storeHues(int attributeNumber, const std::vector<float>& hues)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, hues.size() * sizeof(float), hues.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Loader::storeColors(int attributeNumber, const std::vector<float>& data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Loader::unbindVAO()
{
    glBindVertexArray(0);
}

void Loader::CleanUp()
{
    if (!vaos.empty())
        glDeleteVertexArrays(vaos.size(), vaos.data());

    if (!vbos.empty())
        glDeleteBuffers(vbos.size(), vbos.data());
}

void Loader::bindIndicesBuffer(const std::vector<int>& indices)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
}
