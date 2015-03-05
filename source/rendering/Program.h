#pragma once

#include <GL/glew.h>

#include "./Shader.h"

class Program
{
public:
    bool init(const Shader& vertex, const Shader& fragment);
    GLuint getAttribute(const char* attribute);
    GLuint getUniform(const char* attribute);
    void setActive();
    void destroy();

    static Program* createProgram(const std::string& vertexSource, const std::string& fragmentSource);

private:
    GLuint _id;
};