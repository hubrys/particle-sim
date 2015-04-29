#pragma once
#ifndef cuda_utils_h
#define cuda_utils_h
//#include <cmath>
#include "cuda_runtime.h"

#define GRAV_CONST 1
#define BLOCK_SIZE 256
#define MIN_CALC_DISTANCE .01f
#define PARTICLES_PER_THREAD 4

__device__ float2 add(float2 lhs, float2 rhs);
__device__ float length(float2 vec);
__device__ float2 subtract(float2 lhs, float2 rhs);
__device__ float2 normalize(float2 vec);
__device__ float2 scale(float2 vec, float scale);
__device__ float2 calculateForce(float x1, float y1, float m1, float x2, float y2, float m2);
__device__ float2 calculateForceMouse(float x1, float y1, float m1, float x2, float y2, float m2);

#endif