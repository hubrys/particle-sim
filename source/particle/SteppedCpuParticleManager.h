#pragma once

#include "./CpuParticleManager.h"

class SteppedCpuParticleManager
	: public CpuParticleManager
{
public:
	virtual void tick(float deltaTime, float particleMass, glm::vec2 mousePos, float mouseMass);
};