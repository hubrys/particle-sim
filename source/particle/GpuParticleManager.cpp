#include "./GpuParticleManager.h"

void launchCudaTick(float deltaTime, float particleMass, float mouseX, float mouseY, float mouseMass,
struct cudaGraphicsResource* particleResource, bool mouseOnly, int particleCount, float2 bounds, float friction);

const char* GpuParticleManager::init()
{
    const char* result = CpuParticleManager::init();

    if (result != nullptr)
    {
        return result;
    }

    _particleDimX = Config::instance()->getInt("particleCountX", 32);
    _particleDimY = Config::instance()->getInt("particleCountY", 32);
    cudaError_t cudaResult = cudaGraphicsGLRegisterBuffer(&_cuda_particles, _d_particleBuffer, cudaGraphicsRegisterFlagsNone);
    if (cudaResult != cudaSuccess)
    {
        return "could not register buffer";
    }

    glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _particleCount * sizeof(CpuParticle), _particles);

    return nullptr;
}

void GpuParticleManager::tick(float deltaTime, float particleMass, glm::vec2 mousePos, float mouseMass)
{
    launchCudaTick(deltaTime, 
    particleMass, 
    mousePos.x, mousePos.y, 
    mouseMass, 
    _cuda_particles, 
    _mouseOnly,
    _particleDimX * _particleDimY,
    make_float2(_bounds.x, _bounds.y),
    _friction);
}

void GpuParticleManager::render(float deltaTime)
{
    glUniform1f(_d_deltaTime, deltaTime);
    glUniformMatrix4fv(_d_projection, 1, GL_FALSE, glm::value_ptr(_projection));
    //// transfer data to GPU 
    //glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, _particleCount * sizeof(CpuParticle), _particles);

    glBindVertexArray(_d_vao);
    glDrawArrays(GL_POINTS, 0, _particleCount);
}