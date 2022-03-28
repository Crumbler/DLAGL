#include "PointShader.hpp"

constexpr char PointShader::VERTEX_SHADER[];
constexpr char PointShader::FRAGMENT_SHADER[];

void PointShader::Init(const bool useHue, const float hueS,
                       const float hueV, const float hueMult)
{
    ShaderProgram::Init({ ShaderInfo(VERTEX_SHADER, GL_VERTEX_SHADER),
                          ShaderInfo(FRAGMENT_SHADER, GL_FRAGMENT_SHADER)});

    glUseProgram(programID);
    setUseHue(useHue);
    setHueS(hueS);
    setHueV(hueV);
    setHueMult(hueMult);
    glUseProgram(0);
}

void PointShader::setUseHue(const bool useHue)
{
    if (useHue)
        loadFloat(loc_useHue, 1.0f);
    else
        loadFloat(loc_useHue, 0.0f);
}

void PointShader::setHueS(const float S)
{
    loadFloat(loc_hueS, S);
}

void PointShader::setHueV(const float V)
{
    loadFloat(loc_hueV, V);
}

void PointShader::setHueMult(const float mult)
{
    loadFloat(loc_hueMult, mult);
}

void PointShader::bindAttributes()
{
    bindAttribute(0, "position");
    bindAttribute(1, "VHue");
}

void PointShader::getAllUniformLocations()
{
    loc_projMat = getUniformLocation("projMat");
    loc_useHue = getUniformLocation("useHue");
    loc_hueS = getUniformLocation("hueS");
    loc_hueV = getUniformLocation("hueV");
    loc_hueMult = getUniformLocation("hueMult");
}

void PointShader::loadProjectionMatrix(const glm::mat4& mat)
{
    loadMatrix(loc_projMat, mat);
}
