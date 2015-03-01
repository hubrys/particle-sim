#pragma once

#include <GL/glew.h>

#include <string>
#include <fstream>

/// Represents an opengl shader
class Shader
{
public:
    enum Type : GLenum { Vertex = GL_VERTEX_SHADER, Fragment = GL_FRAGMENT_SHADER};

    bool init(const std::string& shaderSource, Type shaderType);
    GLuint id() const { return _id; }

private:
    GLuint _id;
};