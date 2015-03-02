#include "./SteppedCpuParticleManager.h"

void SteppedCpuParticleManager::tick(float deltaTime)
{
	for (int particleI = 0; particleI < _particleCount - 1; particleI++)
	{
		CpuParticle* particle = &_particles[particleI];

        for (int otherParticleI = particleI + 1; otherParticleI < _particleCount; otherParticleI++)
        {
            glm::vec2 diff = _particles[otherParticleI].position - particle->position;
            float distance = glm::length(diff);
            float force = _gravConst / ((distance * distance) + _minCalcDistance);
            diff = force * glm::normalize(diff);
            particle->velocity += diff;
            _particles[otherParticleI].velocity -= diff;
        }
	}

    for (int particleI = 0; particleI < _particleCount; particleI++)
    {
        _particles[particleI].position += _particles[particleI].velocity * deltaTime;
    }
}