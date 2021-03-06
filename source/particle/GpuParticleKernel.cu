#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_utils.h"
#include <iostream>

typedef struct
{
    int count;
    float deltaTime;
    float particleMass;
    float2 mousePos;
    float mouseMass;
    float friction;
    float2 bounds;
} KernelArgs;

__global__ void tickMouseOnly(const float4* input, float4* output, KernelArgs args)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    float4 particle = input[index];

    float2 velocity = scale(
        calculateForceMouse(particle.x, particle.y, args.particleMass, args.mousePos.x, args.mousePos.y, args.mouseMass),
        args.deltaTime
        );

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


    //if (args.bounds.x - abs(position.x) < 0 &&
    //    position.x * velocity.x > 0)
    //{
    //    //velocity.x = -velocity.x;
    //    position.x = position.x > 0 ? -args.bounds.x : args.bounds.x;
    //}

    //if (args.bounds.y - abs(position.y) < 0 &&
    //    position.y * velocity.y > 0)
    //{
    //    //velocity.y = -velocity.y;
    //    position.y = position.y > 0 ? -args.bounds.y : args.bounds.y;
    //}

    velocity = scale(velocity, 1 - (args.friction * args.deltaTime));

    output[index] = make_float4(position.x, position.y,
                                        velocity.x, velocity.y);
}

__global__ void tickNBody(const float4* __restrict__ input, float4* __restrict__ output, KernelArgs args)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    float4 particle = input[index];
    float2 thisPosition = make_float2(particle.x, particle.y);

    float2 force = calculateForceMouse(particle.x, particle.y, args.particleMass,
                                       args.mousePos.x, args.mousePos.y, args.mouseMass);


    //// Calculate particle forces
    //__shared__ float4 particles[BLOCK_SIZE];
    //for (int particleI = 0; particleI < args.count; particleI += BLOCK_SIZE)
    //{
    //    particles[threadIdx.x] = args.input[particleI + threadIdx.x];
    //    __syncthreads();

    //    for (int subI = 0; subI < BLOCK_SIZE; subI += 1)
    //    {
    //        force = add(force, calculateForce(
    //            particle.x, particle.y, args.particleMass,
    //            particles[subI].x, particles[subI].y, args.particleMass)
    //            );

    //        //force = add(force, calculateForce(
    //        //    particle.x, particle.y, args.particleMass,
    //        //    particles[subI + 1].x, particles[subI + 1].y, args.particleMass)
    //        //    );

    //        //force = add(force, calculateForce(
    //        //    particle.x, particle.y, args.particleMass,
    //        //    particles[subI + 2].x, particles[subI + 2].y, args.particleMass)
    //        //    );

    //        //force = add(force, calculateForce(
    //        //    particle.x, particle.y, args.particleMass,
    //        //    particles[subI + 3].x, particles[subI + 3].y, args.particleMass)
    //        //    );
    //    }
    //}

    float2 diff;
    float distance;
    float magnitude;
    for (int particleI = 0; particleI < args.count; particleI ++)
    {
        if (particleI != index)
        {
        diff.x = input[particleI].x - particle.x;
        diff.y = input[particleI].y - particle.y;

        distance = length(diff) + MIN_CALC_DISTANCE;
        magnitude = (GRAV_CONST * args.particleMass * args.particleMass) / (distance * distance);
        force = add(force, scale(normalize(diff), magnitude));
        }
    }

    // Calc resulting velocity
    float2 velocity = make_float2(particle.z, particle.w);
    velocity = add(velocity, scale(force, args.deltaTime)); // velocty += force * deltaTime

    float2 position = make_float2(particle.x, particle.y);
    position = add(position, scale(velocity, args.deltaTime));

    /*if (args.bounds.x - abs(position.x) < 0 &&
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
        */
    if (args.bounds.x - abs(position.x) < 0 &&
        position.x * velocity.x > 0)
    {
        //velocity.x = -velocity.x;
        position.x = position.x > 0 ? -args.bounds.x : args.bounds.x;
    }

    if (args.bounds.y - abs(position.y) < 0 &&
        position.y * velocity.y > 0)
    {
        //velocity.y = -velocity.y;
        position.y = position.y > 0 ? -args.bounds.y : args.bounds.y;
    }

    velocity = scale(velocity, 1 - args.friction * args.deltaTime);

    output[index] = make_float4(position.x, position.y,
                                        velocity.x, velocity.y);
}


void launchCudaTick(float deltaTime, float particleMass, float mouseX, float mouseY, float mouseMass,
struct cudaGraphicsResource* particleResource, bool sourceBufferFirst, bool mouseOnly, int particleCount, float2 bounds, float friction)
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

    result = cudaGraphicsResourceGetMappedPointer((void**) &particles, &numBytes, particleResource);
    if (result != cudaSuccess)
    {
        printf("failed grabbing resource\n");
        exit(0);
    }

    args.count = particleCount;
    args.deltaTime = deltaTime;
    args.particleMass = particleMass;
    args.mousePos = make_float2(mouseX, mouseY);
    args.mouseMass = mouseMass;
    args.friction = friction;
    args.bounds = bounds;

    dim3 blockDim(BLOCK_SIZE, 1, 1);
    dim3 gridDim(particleCount / BLOCK_SIZE, 1, 1);

    if (mouseOnly)
    {
        tickMouseOnly<<<gridDim, blockDim>>>(&particles[sourceBufferFirst ? 0 : particleCount],
                                             &particles[sourceBufferFirst ? particleCount : 0],
                                             args);
    }
    else
    {
        tickNBody<<<gridDim, blockDim>>>(&particles[sourceBufferFirst ? 0 : particleCount],
                                         &particles[sourceBufferFirst ? particleCount : 0],
                                         args);
    }

    cudaDeviceSynchronize();

    result = cudaGraphicsUnmapResources(1, &particleResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed unmapping resource\n");
        exit(0);
    }
}