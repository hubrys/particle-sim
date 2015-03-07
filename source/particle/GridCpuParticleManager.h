#pragma once

#include "./CpuParticleManager.h"

class GridCpuParticleManager
	: public CpuParticleManager
{
public:
	virtual void tick(float deltaTime, glm::vec2 mousePos, float mouseMass);
};