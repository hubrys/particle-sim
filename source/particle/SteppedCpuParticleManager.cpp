#include "./SteppedCpuParticleManager.h"

void SteppedCpuParticleManager::tick(float deltaTime, float particleMass, glm::vec2 mousePosition, float mouseMass)
{
	for (int particleI = 0; particleI < _particleCount - 1; particleI++)
	{
        _particles[particleI].velocity += calculateMouseForce(_particles[particleI], mousePosition, mouseMass) * particleMass * deltaTime;

		if (_mouseOnly == false)
		{
	        for (int otherParticleI = particleI + 1; otherParticleI < _particleCount; otherParticleI++)
	        {
	            glm::vec2 force = calculateForce(_particles[particleI], _particles[otherParticleI]) * particleMass * particleMass * deltaTime;
	            _particles[particleI].velocity += force;
	            _particles[otherParticleI].velocity -= force;
	        }
		}
	}

	applyVelocity(deltaTime);
}