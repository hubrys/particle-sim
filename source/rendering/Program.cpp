#include "./Program.h"

bool Program::init(const Shader& vertex, const Shader& fragment)
{
    _id = glCreateProgram();

    // attach the shaders
    glAttachShader(_id, vertex.id());
    glAttachShader(_id, fragment.id());

    // link the program
    glLinkProgram(_id);

    GLint isLinked = 0;
    glGetProgramiv(_id, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &maxLength);
        char* log = new char[maxLength];
        glGetProgramInfoLog(_id, maxLength, &maxLength, log);
        printf("ERROR PROGRAM LINKING: %s", log);
        delete[] log;

        glDeleteProgram(_id);
        return false;
    }

    glDetachShader(_id, vertex.id());
    glDetachShader(_id, fragment.id());
    return true;
}

void Program::destroy() 
{
    glDeleteProgram(_id);
}

GLuint Program::getAttribute(const char* attribute)
{
    return glGetAttribLocation(_id, attribute);
}

GLuint Program::getUniform(const char* uniform)
{
    return glGetUniformLocation(_id, uniform);
}

void Program::setActive() 
{ 
    glUseProgram(_id);
}

Program* Program::createProgram(const std::string& vertexSource, const std::string& fragmentSource)
{
    Shader vertex;
    Shader fragment;

    if (vertex.init(vertexSource, Shader::Type::Vertex) == false)
    {
        printf("failed to create vertex shader\n");
        return nullptr;
    }

    if (fragment.init(fragmentSource, Shader::Type::Fragment) == false)
    {
        printf("failed to create fragment shader\n");
        return nullptr;
    }

    Program* program = new Program();
    if (program->init(vertex, fragment) == false)
    {
        delete program;
        return nullptr;
    }

    return program;
}