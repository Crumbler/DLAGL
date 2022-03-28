#pragma once

#include "DLABase.hpp"
#include "ShaderProgram.hpp"

class DLABaseShader : public ShaderProgram
{
    public:
        void Init(const bool useHue, const float hueS,
                  const float hueV);
        void loadProjectionMatrix(const glm::mat4& mat);
        void setUseHue(const bool useHue);
        void setHueS(const float S);
        void setHueV(const float V);
        void setLineCount(const int count);
        void setLines(const std::vector<Line>& lines);
        void setCircles(const std::vector<Circle>& circles);
        void setCircleCount(const int count);
        void setBoxes(const std::vector<Box>& boxes);
        void setBoxCount(const int count);
        void setBeziers(const std::vector<Bezier>& beziers);
        void setBezierCount(const int count);
    protected:
        void bindAttributes();
        void getAllUniformLocations();
    private:
        GLint loc_projMat, loc_useHue, loc_hueS, loc_hueV,
              loc_Lines, loc_lineCount, loc_Circles, loc_circleCount,
              loc_Boxes, loc_boxCount, loc_Beziers, loc_bezierCount;
        static constexpr char VERTEX_SHADER[] = "vDLABaseShader.txt",
                              FRAGMENT_SHADER[] = "fDLABaseShader.txt";
};
