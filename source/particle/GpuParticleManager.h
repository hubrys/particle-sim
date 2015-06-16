#pragma once
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../rendering/Program.h"
#include "IParticleManager.h"
#include "cuda_runtime.h"
#include "cuda_gl_interop.h"

class GpuParticleManager
    : public IParticleManager
{
public: 
    virtual const char* init();
    virtual void tick(float deltaTime, float particleMass, glm::vec2 mousePos, float mouseMass);
    virtual void render(float deltaTime);

protected:
    struct cudaGraphicsResource* _cuda_particles;
    GLuint _d_buffer;
    GLuint _d_vao;
    bool _sourceBufferFirst;

    int _particleCount;
    int _particleDimX;
    int _particleDimY;

    bool _mouseOnly;
    float _gravConst;
    float _scale;
    float _minCalcDistance;
    float _mouseMass;
    float _friction;

    glm::vec2 _bounds;

    Program* _program;
    GLuint _d_projection;
    GLuint _d_deltaTime;
    glm::mat4 _projection;
};