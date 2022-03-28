#include "Renderer.hpp"
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::Prepare()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::RenderPoints(const PointModel& model)
{
    glBindVertexArray(model.getVaoID());
    glDrawArrays(GL_POINTS, 0, model.getVertexCount());
    glBindVertexArray(0);
}

void Renderer::RenderDLABase(const RawModel& model)
{
    glBindVertexArray(model.getVaoID());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, model.getVertexCount());
    glBindVertexArray(0);
}

void Renderer::Render(const RawModel& model)
{
    glBindVertexArray(model.getVaoID());
    glDrawArrays(GL_POINTS, 0, model.getVertexCount());
    glBindVertexArray(0);
}

void Renderer::updateProjectionMatrix(ShaderProgram& shader, const glm::mat4& mat)
{
    shader.Start();
    shader.loadProjectionMatrix(mat);
    shader.Stop();
}

Renderer::Renderer() {}
