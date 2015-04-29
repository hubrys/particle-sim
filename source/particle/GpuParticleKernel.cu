#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_utils.h"
#include <iostream>

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

__global__ void tickMouseOnly(KernelArgs args)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    float4 particle = args.particles[index];

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

    args.particles[index] = make_float4(position.x, position.y,
                                        velocity.x, velocity.y);
}

__global__ void tickNBody(KernelArgs args)
{
    int index = PARTICLES_PER_THREAD * blockIdx.x * blockDim.x + threadIdx.x;

    float4 particles[PARTICLES_PER_THREAD];
    particles[0] = args.particles[index];
    particles[1] = args.particles[index + BLOCK_SIZE];
    particles[2] = args.particles[index + 2 * BLOCK_SIZE];
    particles[3] = args.particles[index + 3 * BLOCK_SIZE];

    float2 forces[PARTICLES_PER_THREAD];
    forces[0] = calculateForceMouse(particles[0].x, particles[0].y, args.particleMass,
                                  args.mousePos.x, args.mousePos.y, args.mouseMass);
    forces[1] = calculateForceMouse(particles[1].x, particles[1].y, args.particleMass,
                                    args.mousePos.x, args.mousePos.y, args.mouseMass);
    forces[2] = calculateForceMouse(particles[2].x, particles[2].y, args.particleMass,
                                    args.mousePos.x, args.mousePos.y, args.mouseMass);
    forces[3] = calculateForceMouse(particles[3].x, particles[3].y, args.particleMass,
                                    args.mousePos.x, args.mousePos.y, args.mouseMass);


    // Calculate particle forces
    __shared__ float4 otherParticles[BLOCK_SIZE];
    for (int particleI = 0; particleI < args.count; particleI += BLOCK_SIZE)
    {
        otherParticles[threadIdx.x] = args.particles[particleI + threadIdx.x];
        __syncthreads();

        for (int subI = 0; subI < BLOCK_SIZE; subI++)
        {
            forces[0] = add(forces[0], calculateForce(
                particles[0].x, particles[0].y, args.particleMass,
                otherParticles[subI].x, otherParticles[subI].y, args.particleMass)
                );
            forces[1] = add(forces[1], calculateForce(
                particles[1].x, particles[1].y, args.particleMass,
                otherParticles[subI].x, otherParticles[subI].y, args.particleMass)
                );
            forces[2] = add(forces[2], calculateForce(
                particles[2].x, particles[2].y, args.particleMass,
                otherParticles[subI].x, otherParticles[subI].y, args.particleMass)
                );
            forces[3] = add(forces[3], calculateForce(
                particles[3].x, particles[3].y, args.particleMass,
                otherParticles[subI].x, otherParticles[subI].y, args.particleMass)
                );
        }
    }

    // Calc resulting velocity
    float2 velocities[PARTICLES_PER_THREAD];
    velocities[0] = add(make_float2(particles[0].z, particles[0].w), 
                    scale(forces[0], args.deltaTime)); // velocty += force * deltaTime
    velocities[1] = add(make_float2(particles[1].z, particles[1].w),
                        scale(forces[1], args.deltaTime)); // velocty += force * deltaTime
    velocities[2] = add(make_float2(particles[2].z, particles[2].w),
                        scale(forces[2], args.deltaTime)); // velocty += force * deltaTime
    velocities[3] = add(make_float2(particles[3].z, particles[3].w),
                        scale(forces[3], args.deltaTime)); // velocty += force * deltaTime

    float2 positions[PARTICLES_PER_THREAD];
    positions[0] = add(make_float2(particles[0].x, particles[0].y),
                       scale(velocities[0], args.deltaTime));
    positions[1] = add(make_float2(particles[1].x, particles[1].y),
                       scale(velocities[1], args.deltaTime));
    positions[2] = add(make_float2(particles[2].x, particles[2].y),
                       scale(velocities[2], args.deltaTime));
    positions[3] = add(make_float2(particles[3].x, particles[3].y),
                       scale(velocities[3], args.deltaTime));

    // bounds checking
    if (args.bounds.x - abs(positions[0].x) < 0 &&
        positions[0].x * velocities[1].x > 0)
    {
        //velocity.x = -velocity.x;
        positions[0].x = positions[0].x > 0 ? -args.bounds.x : args.bounds.x;
    }
    if (args.bounds.y - abs(positions[0].y) < 0 &&
        positions[0].y * velocities[1].y > 0)
    {
        //velocity.y = -velocity.y;
        positions[0].y = positions[0].y > 0 ? -args.bounds.y : args.bounds.y;
    }

    if (args.bounds.x - abs(positions[1].x) < 0 &&
        positions[1].x * velocities[1].x > 0)
    {
        //velocity.x = -velocity.x;
        positions[1].x = positions[1].x > 0 ? -args.bounds.x : args.bounds.x;
    }
    if (args.bounds.y - abs(positions[1].y) < 0 &&
        positions[1].y * velocities[1].y > 0)
    {
        //velocity.y = -velocity.y;
        positions[1].y = positions[1].y > 0 ? -args.bounds.y : args.bounds.y;
    }

    if (args.bounds.x - abs(positions[2].x) < 0 &&
        positions[2].x * velocities[2].x > 0)
    {
        //velocity.x = -velocity.x;
        positions[2].x = positions[2].x > 0 ? -args.bounds.x : args.bounds.x;
    }
    if (args.bounds.y - abs(positions[2].y) < 0 &&
        positions[2].y * velocities[2].y > 0)
    {
        //velocity.y = -velocity.y;
        positions[2].y = positions[2].y > 0 ? -args.bounds.y : args.bounds.y;
    }

    if (args.bounds.x - abs(positions[3].x) < 0 &&
        positions[3].x * velocities[3].x > 0)
    {
        //velocity.x = -velocity.x;
        positions[3].x = positions[3].x > 0 ? -args.bounds.x : args.bounds.x;
    }
    if (args.bounds.y - abs(positions[3].y) < 0 &&
        positions[3].y * velocities[3].y > 0)
    {
        //velocity.y = -velocity.y;
        positions[3].y = positions[3].y > 0 ? -args.bounds.y : args.bounds.y;
    }

    // Apply friction
    velocities[0] = scale(velocities[0], 1 - args.friction * args.deltaTime);
    velocities[1] = scale(velocities[1], 1 - args.friction * args.deltaTime);
    velocities[2] = scale(velocities[2], 1 - args.friction * args.deltaTime);
    velocities[3] = scale(velocities[3], 1 - args.friction * args.deltaTime);

    args.particles[index] = make_float4(positions[0].x, positions[0].y,
                                        velocities[0].x, velocities[0].y);
    args.particles[index + BLOCK_SIZE] = make_float4(positions[1].x, positions[1].y,
                                        velocities[1].x, velocities[1].y);
    args.particles[index + 2 * BLOCK_SIZE] = make_float4(positions[2].x, positions[2].y,
                                                     velocities[2].x, velocities[2].y);
    args.particles[index + 3 * BLOCK_SIZE] = make_float4(positions[3].x, positions[3].y,
                                                     velocities[3].x, velocities[3].y);
}


void launchCudaTick(float deltaTime, float particleMass, float mouseX, float mouseY, float mouseMass,
struct cudaGraphicsResource* particleResource, bool mouseOnly, int particleCount, float2 bounds, float friction)
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

    args.particles = particles;
    args.count = particleCount;
    args.deltaTime = deltaTime;
    args.particleMass = particleMass;
    args.mousePos = make_float2(mouseX, mouseY);
    args.mouseMass = mouseMass;
    args.friction = friction;
    args.bounds = bounds;

    dim3 blockDim(BLOCK_SIZE, 1, 1);
    dim3 gridDim(particleCount / (BLOCK_SIZE * PARTICLES_PER_THREAD), 1, 1);

    if (mouseOnly)
    {
        tickMouseOnly<<<gridDim, blockDim>>>(args);
    }
    else
    {
        tickNBody<<<gridDim, blockDim>>>(args);
    }

    cudaDeviceSynchronize();

    result = cudaGraphicsUnmapResources(1, &particleResource, 0);
    if (result != cudaSuccess)
    {
        printf("failed unmapping resource\n");
        exit(0);
    }
}