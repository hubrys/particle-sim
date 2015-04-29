#include "cuda_utils.h"

__device__ float2 add(float2 lhs, float2 rhs)
{
    return make_float2(lhs.x + rhs.x, lhs.y + rhs.y);
}

__device__ float length(float2 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

__device__ float2 subtract(float2 lhs, float2 rhs)
{
    return make_float2(lhs.x - rhs.x, lhs.y - rhs.y);
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

__device__ float2 calculateForce(float x1, float y1, float m1, float x2, float y2, float m2)
{
    float2 diff = make_float2(x2 - x1, y2 - y1);
    float distance = length(diff);
    if (distance != 0)
    {
        distance += MIN_CALC_DISTANCE;
        float magnitude = GRAV_CONST * m1 * m2 / (distance * distance);
        return scale(normalize(diff), magnitude);
    }
    else
    {
        return make_float2(0, 0);
    }
}

__device__ float2 calculateForceMouse(float x1, float y1, float m1, float x2, float y2, float m2)
{
    float2 diff = make_float2(x2 - x1, y2 - y1);
    float distance = length(diff);
    if (distance > 0)
    {
        distance += MIN_CALC_DISTANCE;
        float magnitude = GRAV_CONST * m1 * m2 / distance;
        return scale(normalize(diff), magnitude);
    }
    else
    {
        return make_float2(0, 0);
    }
}