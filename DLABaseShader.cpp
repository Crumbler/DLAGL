#include "DLABaseShader.hpp"

constexpr char DLABaseShader::VERTEX_SHADER[];
constexpr char DLABaseShader::FRAGMENT_SHADER[];

void DLABaseShader::Init(const bool useHue, const float hueS,
                  const float hueV)
{
    ShaderProgram::Init({ ShaderInfo(VERTEX_SHADER, GL_VERTEX_SHADER),
                          ShaderInfo(FRAGMENT_SHADER, GL_FRAGMENT_SHADER)});

    glUseProgram(programID);

    setUseHue(useHue);
    setHueS(hueS);
    setHueV(hueV);
    setLineCount(0);
    setCircleCount(0);
    setBoxCount(0);
    setBezierCount(0);
    glUseProgram(0);
}

void DLABaseShader::bindAttributes()
{
    bindAttribute(0, "position");
    bindAttribute(1, "Vuv");
}

void DLABaseShader::getAllUniformLocations()
{
    loc_projMat = getUniformLocation("projMat");
    loc_useHue = getUniformLocation("useHue");
    loc_hueS = getUniformLocation("hueS");
    loc_hueV = getUniformLocation("hueV");
    loc_Lines = getUniformLocation("Lines");
    loc_lineCount = getUniformLocation("lineCount");
    loc_Circles = getUniformLocation("Circles");
    loc_circleCount = getUniformLocation("circleCount");
    loc_Boxes = getUniformLocation("Boxes");
    loc_boxCount = getUniformLocation("boxCount");
    loc_Beziers = getUniformLocation("Beziers");
    loc_bezierCount = getUniformLocation("bezierCount");
}

void DLABaseShader::setUseHue(const bool useHue)
{
    if (useHue)
        loadFloat(loc_useHue, 1.0f);
    else
        loadFloat(loc_useHue, 0.0f);
}

void DLABaseShader::setHueS(const float S)
{
    loadFloat(loc_hueS, S);
}

void DLABaseShader::setHueV(const float V)
{
    loadFloat(loc_hueV, V);
}

void DLABaseShader::setLineCount(const int count)
{
    loadInt(loc_lineCount, count);
}

void DLABaseShader::setLines(const std::vector<Line>& lines)
{
    setLineCount(lines.size());
    glUniform4fv(loc_Lines, lines.size(), (const float*)lines.data());
}

void DLABaseShader::setCircles(const std::vector<Circle>& circles)
{
    setCircleCount(circles.size());
    glUniform4fv(loc_Circles, circles.size(), (const float*)circles.data());
}

void DLABaseShader::setCircleCount(const int count)
{
    loadInt(loc_circleCount, count);
}

void DLABaseShader::setBoxes(const std::vector<Box>& boxes)
{
    setBoxCount(boxes.size());
    glUniform1fv(loc_Boxes, boxes.size() * 5, (const float*)boxes.data());
}

void DLABaseShader::setBoxCount(const int count)
{
    loadInt(loc_boxCount, count);
}

void DLABaseShader::setBeziers(const std::vector<Bezier>& beziers)
{
    setBezierCount(beziers.size());
    glUniformMatrix3x2fv(loc_Beziers, beziers.size(),GL_FALSE, (const float*)beziers.data());
}

void DLABaseShader::setBezierCount(const int count)
{
    loadInt(loc_bezierCount, count);
}

void DLABaseShader::loadProjectionMatrix(const glm::mat4& mat)
{
    loadMatrix(loc_projMat, mat);
}
