#pragma once

#include "PointShader.hpp"
#include "DLABaseShader.hpp"
#include "RawModel.hpp"
#include "PointModel.hpp"
#include <glm/glm.hpp>

class Renderer
{
    public:
        Renderer();
        void RenderPoints(const PointModel& model);
        void RenderDLABase(const RawModel& model);
        void Render(const RawModel& model);
        void Prepare();
        void updateProjectionMatrix(ShaderProgram& shader, const glm::mat4& mat);

        static constexpr float FOV = 90.0f,
                               NEAR_PLANE = 0.1f,
                               FAR_PLANE = 100.0f,
                               FOV_R = FOV / 180.0f * acos(-1.0f);
};
