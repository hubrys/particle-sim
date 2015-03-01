#include "./Shader.h"
#include <cassert>

bool Shader::init(const std::string& shaderSource, Shader::Type shaderType)
{
    // Read shader source into memory
    std::ifstream source(shaderSource);
    if (source.is_open() == false)
    {
        printf("ERROR: could not open file %s", shaderSource.c_str());
        return false;
    }

    // get length of source file
    source.seekg(0, source.end);
    int fileLength = static_cast<int>(source.tellg());
    source.seekg(0, source.beg);

    // allocate buffer, leave space for '\0'
    char* buffer = new char[fileLength];

    // read entire file as a single block
    source.read(buffer, fileLength);
	assert(source.gcount() == fileLength); //XXX has issues with size

    source.close();

    // Setup OpenGL members
    _id = glCreateShader(shaderType);
    if (_id == 0)
    {
        delete[] buffer;
        return false;
    }

    // compile shader
    glShaderSource(_id, 1, &buffer, &fileLength);
    glCompileShader(_id);

    // cleanup buffer
    delete[] buffer;

    // check compilation result
    GLint success = 0;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) 
    {
        GLint maxLength = 0;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char* errorLog = new char[maxLength];
        glGetShaderInfoLog(_id, maxLength, &maxLength, errorLog);
        printf("SHADER COMPILATION ERROR: %s", errorLog);
        delete[] errorLog;

        // cleanup shader
        glDeleteShader(_id);
        return false;
    }

    return true;
}