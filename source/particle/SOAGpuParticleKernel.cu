#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_utils.h"
#include <iostream>

typedef struct
{
    float2* positions;
    float2* velocities;
    int count;
    float deltaTime;
    float particleMass;
    float2 mousePos;
    float mouseMass;
    float friction;
    float2 bounds;
} SOAKernelArgs;

__global__ void tickMouseOnly(SOAKernelArgs args)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    float2 position = args.positions[index];
    float2 velocity = add(args.velocities[index],
        scale(
        calculateForceMouse(position.x, position.y, args.particleMass, args.mousePos.x, args.mousePos.y, args.mouseMass),
        args.deltaTime
        ));


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

    velocity = scale(velocity, 1 - args.friction * args.deltaTime);

    args.positions[index] = position;
    args.velocities[index] = velocity;
}

__global__ void tickNBody(SOAKernelArgs args)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    float2 position = args.positions[index];
    float2 force = calculateForceMouse(position.x, position.y, args.particleMass,
                                  args.mousePos.x, args.mousePos.y, args.mouseMass);

    //// Calculate particle forces
    //__shared__ float2 positions[BLOCK_SIZE];
    //for (int particleI = 0; particleI < args.count; particleI += BLOCK_SIZE)
    //{
    //    positions[threadIdx.x] = args.positions[particleI + threadIdx.x];
    //    __syncthreads();

    //    for (int subI = 0; subI < BLOCK_SIZE; subI++)
    //    {
    //        force = add(force, calculateForce(
    //            position.x, position.y, args.particleMass,
    //            positions[subI].x, positions[subI].y, args.particleMass)
    //            );
    //    }
    //}

    for (int particleI = 0; particleI < args.count; particleI ++)
    {
    if (particleI != index)
    {
        force = add(force, calculateForce(
                        position.x, position.y, args.particleMass,
                        args.positions[particleI].x, args.positions[particleI].y, args.particleMass)
                        );
    }
    }


    // Calc resulting velocity
    float2 velocity = add(args.velocities[index],
                        scale(force, args.deltaTime)); // velocty += force * deltaTime
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

    args.positions[index] = position;
    args.velocities[index] = velocity;
}


void launchSOACudaTick(float deltaTime, float particleMass, float mouseX, float mouseY, float mouseMass,
struct cudaGraphicsResource* positionsResource, struct cudaGraphicsResource* velocitiesResource, bool mouseOnly, int particleCount, float2 bounds, float friction)
{
    size_t numBytes;
    SOAKernelArgs args;
    float2* positions;
    float2* velocities;
    cudaError_t result = cudaGraphicsMapResources(1, &positionsResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed mapping resource\n");
        exit(0);
    }

    result = cudaGraphicsResourceGetMappedPointer((void**) &positions, &numBytes, positionsResource);
    if (result != cudaSuccess)
    {
        printf("failed grabbing resource\n");
        exit(0);
    }

    result = cudaGraphicsMapResources(1, &velocitiesResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed mapping v resource\n");
        exit(0);
    }

    result = cudaGraphicsResourceGetMappedPointer((void**) &velocities, &numBytes, velocitiesResource);
    if (result != cudaSuccess)
    {
        printf("failed grabbing v resource\n");
        exit(0);
    }

    args.positions = positions;
    args.velocities = velocities;
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
        tickMouseOnly<<<gridDim, blockDim>>>(args);
    }
    else
    {
        tickNBody<<<gridDim, blockDim>>>(args);
    }

    cudaDeviceSynchronize();

    result = cudaGraphicsUnmapResources(1, &positionsResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed unmapping resource\n");
        exit(0);
    }

    result = cudaGraphicsUnmapResources(1, &velocitiesResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed unmapping v resource\n");
        exit(0);
    }
}