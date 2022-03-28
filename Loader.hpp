#pragma once

#include "RawModel.hpp"
#include "PointModel.hpp"
#include <glm/glm.hpp>
#include <vector>

class Loader
{
    public:
        Loader();
        PointModel loadPointsEmpty();
        RawModel loadToVAO(const std::vector<float>& positions,
                           const std::vector<float>& colors);
        RawModel loadDLABase(const std::vector<float>& positions,
                             const std::vector<float>& uvs);
        void CleanUp();

    private:
        std::vector<GLuint> vaos, vbos;

        int createVAO();
        int storeEmptyHues(int attributeNumber);
        int storeEmptyPointPositions(int attributeNumber);
        void storeDataInAttributeList(int attributeNumber, const std::vector<float>& data);
        void storeDynamic(int attributeNumber, const std::vector<float>& data);
        void storeStatic(int attributeNumber, const std::vector<float>& data);
        void storeColors(int attributeNumber, const std::vector<float>& data);
        void storeHues(int attributeNumber, const std::vector<float>& hues);
        void unbindVAO();

        void bindIndicesBuffer(const std::vector<int>& indices);
};
