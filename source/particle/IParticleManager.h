#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../Config.h"

class IParticleManager
{
public:
	virtual ~IParticleManager() = 0;
	virtual const char* init() = 0;
	virtual void tick(float deltaTime, float particleMass, glm::vec2 mousePos, float mouseMass) = 0;
	virtual void render(float deltaTime) = 0;
};

inline IParticleManager::~IParticleManager() {}