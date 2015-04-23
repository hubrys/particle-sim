#include "cuda_runtime.h"

__device__ float2 add(float2 lhs, float2 rhs)
{
    return make_float2(lhs.x + rhs.x, lhs.y + rhs.y);
}

__device__ float length(float2 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

__device__ float2 normalize(float2 vec)
{
    float l = 1 / length(vec);
    return make_float2(vec.x * l, vec.y * l);
}

__device__ float2 scale(float2 vec, float scale)
{
    return make_float2(vec.x * scale, vec.y * scale);
}