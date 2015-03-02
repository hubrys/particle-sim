#pragma once

#include "./CpuParticleManager.h"

class SteppedCpuParticleManager
	: public CpuParticleManager
{
public:
	virtual void tick(float deltaTime);
};