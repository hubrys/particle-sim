#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_utils.h"
#include <iostream>

#define BLOCK_SIZE 256
#define MIN_CALC_DISTANCE .01f
#define GRAV_CONST 1

typedef struct
{
    float4* particles;
    int count;
    float deltaTime;
    float particleMass;
    float2 mousePos;
    float mouseMass;
    float friction;
    float2 bounds;
} KernelArgs;

__global__ void kernelTick(KernelArgs args)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    float4 particle = args.particles[index];

    // calculate position diff
    float2 diff;
    diff.x = args.mousePos.x - particle.x;
    diff.y = args.mousePos.y - particle.y;

    float distance = length(diff) + MIN_CALC_DISTANCE;
    float magnitude = GRAV_CONST * args.mouseMass * args.particleMass / distance;
    float2 velocity = scale(normalize(diff), magnitude * args.deltaTime);
    velocity.x += particle.z;
    velocity.y += particle.w;
    float2 position = make_float2(particle.x, particle.y);

    position.x += velocity.x * args.deltaTime;
    position.y += velocity.y * args.deltaTime;
    
    if (args.bounds.x - abs(position.x) < 0 &&
        position.x * velocity.x > 0)
    {
        velocity.x = -velocity.x;
        position.x = position.x < 0 ? -args.bounds.x : args.bounds.x;
    }

    if (args.bounds.y - abs(position.y) < 0 &&
        position.y * velocity.y > 0)
    {
        velocity.y = -velocity.y;
        position.y = position.y < 0 ? -args.bounds.y : args.bounds.y;
    }
    
    velocity = scale(velocity, 1 - args.friction * args.deltaTime);

    args.particles[index] = make_float4(position.x, position.y,
                                        velocity.x, velocity.y);
}


void launchCudaTick(float deltaTime, float particleMass, float mouseX, float mouseY, float mouseMass,
struct cudaGraphicsResource* particleResource, bool mouseOnly, int particleCount, float2 bounds)
{
    size_t numBytes;
    KernelArgs args;
    float4* particles;
    cudaError_t result = cudaGraphicsMapResources(1, &particleResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed mapping resource\n");
        exit(0);
    }

    result = cudaGraphicsResourceGetMappedPointer((void**)&particles, &numBytes, particleResource);
    if (result != cudaSuccess)
    {
        printf("failed grabbing resource\n");
        exit(0);
    }

    args.particles = particles;
    args.count = particleCount;
    args.deltaTime = deltaTime;
    args.particleMass = particleMass;
    args.mousePos = make_float2(mouseX, mouseY);
    args.mouseMass = mouseMass;
    args.friction = .9f;
    args.bounds = bounds;

    dim3 blockDim(BLOCK_SIZE, 1, 1);
    dim3 gridDim(particleCount / BLOCK_SIZE, 1, 1);

    kernelTick<<<gridDim, blockDim>>>(args);

    cudaDeviceSynchronize();

    result = cudaGraphicsUnmapResources(1, &particleResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed unmapping resource\n");
        exit(0);
    }
}