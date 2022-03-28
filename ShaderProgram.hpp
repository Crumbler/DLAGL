#pragma once

#include <glad/glad.h>
#include <vector>
#include "ShaderInfo.hpp"
#include <initializer_list>
#include <glm/glm.hpp>

class ShaderProgram
{
    public:

        void Start();
        void Stop();
        void CleanUp();
        virtual void loadProjectionMatrix(const glm::mat4& mat);
        GLuint getProgramID() const;
        void loadFloat(GLint location, float value);

    protected:
        void Init(std::initializer_list<ShaderInfo> shaderInfos);
        virtual void bindAttributes();
        virtual void getAllUniformLocations();
        void bindAttribute(GLuint attribute, const char* variableName);

        void loadInt(GLint location, const int x);
        void loadVector(GLint location, const glm::vec3& vector);
        void loadBoolean(GLint location, bool value);
        void loadMatrix(GLint location, const glm::mat4& value);
        GLint getUniformLocation(const char* uniformName);
        GLuint programID;
    private:
        void inh_bindAttributes();
        void inh_getAllUniformLocations();

        std::vector<int> shaderIds;

    static GLuint loadShader(const char* file, GLenum type);
};
