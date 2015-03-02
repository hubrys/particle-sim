#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class IParticleManager
{
public:
	virtual ~IParticleManager() = 0;
	virtual const char* init(const std::string& assetRoot, int dim, 
		glm::vec3 bounds, bool threeDimensional = false) = 0;
	virtual void tick(float deltaTime) = 0;
	virtual void render() = 0;
};

inline IParticleManager::~IParticleManager() {}