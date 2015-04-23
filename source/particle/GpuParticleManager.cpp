#include "./GpuParticleManager.h"


const char* GpuParticleManager::init()
{
    const char* result = CpuParticleManager::init();

    if (result != nullptr)
    {
        return result;
    }

    cudaGraphicsGLRegisterBuffer(&_cuda_particles, _d_particleBuffer, cudaGraphicsRegisterFlagsNone);
}