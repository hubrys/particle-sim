#pragma once

#include "./CpuParticleManager.h"
#include "cuda_runtime.h"
#include "cuda_gl_interop.h"

class GpuParticleManager
    : public CpuParticleManager
{
public: 
    virtual const char* init();
    virtual void tick(float deltaTime, float particleMass, glm::vec2 mousePos, float mouseMass);
    virtual void render(float deltaTime);

protected:
    struct cudaGraphicsResource* _cuda_particles;
    int _particleDimX;
    int _particleDimY;
};