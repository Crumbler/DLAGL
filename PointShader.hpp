#pragma once

#include "ShaderProgram.hpp"

class PointShader : public ShaderProgram
{
    public:
        void Init(const bool useHue, const float hueS,
                  const float hueV, const float hueMult);
        void loadProjectionMatrix(const glm::mat4& mat);
        void setUseHue(const bool useHue);
        void setHueS(const float S);
        void setHueV(const float V);
        void setHueMult(const float mult);
    protected:
        void bindAttributes();
        void getAllUniformLocations();
    private:
        GLint loc_projMat, loc_useHue, loc_hueS, loc_hueV, loc_hueMult;
        static constexpr char VERTEX_SHADER[] = "vPointShader.txt",
                              FRAGMENT_SHADER[] = "fPointShader.txt";
};
