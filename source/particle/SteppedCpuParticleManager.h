#pragma once

#include "./CpuParticleManager.h"

class SteppedCpuParticleManager
	: public CpuParticleManager
{
public:
	virtual void tick(float deltaTime, glm::vec2 mousePos, float mouseMass);
};