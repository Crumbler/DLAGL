#pragma once

#include <glad/glad.h>
#include <vector>

class RawModel
{
    public:
        RawModel();
        RawModel(GLuint vaoID, GLuint vertexCount);
        GLuint getVaoID() const;
        GLuint getVertexCount() const;
        void updatePoints();
        void updateVBO(int attributeNumber, const std::vector<float>& data);
        void updateVBODynamic(int attributeNumber, const std::vector<float>& data);

    protected:
        GLuint vaoID, vertexCount;
};
