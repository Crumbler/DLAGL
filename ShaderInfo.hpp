#pragma once

struct ShaderInfo
{
    ShaderInfo(const char* file, GLenum type) : file(file), type(type) { };
    const char* file;
    GLenum type;
};
