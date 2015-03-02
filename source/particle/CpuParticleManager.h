#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>

#include "../rendering/Program.h"
#include "./IParticleManager.h"

struct CpuParticle
{
	glm::vec2 position;
	glm::vec2 velocity;

	CpuParticle() : position(0), velocity(0) {}
	CpuParticle(glm::vec2 position, glm::vec2 velocity)
		: position(position)
		, velocity(velocity)
	{
	}
};

class CpuParticleManager 
	: public IParticleManager
{	
public:
	~CpuParticleManager();
	virtual const char* init(const std::string& assestRoot, int dim, 
		glm::vec3 bounds, bool threeDimensional);
	virtual void tick(float deltaTime);
	virtual void render();

private:
	int _particleCount;
	CpuParticle* _particles;

	Program* _program;
	GLuint _d_vao;
	GLuint _d_particleBuffer;
};
