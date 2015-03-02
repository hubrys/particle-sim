#pragma once

#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>

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
	virtual const char* init();
	virtual void tick(float deltaTime);
	virtual void render();

protected:
	int _particleCount;
	CpuParticle* _particles;

	float _gravConst;
	float _scale;

	Program* _program;
	GLuint _d_vao;
	GLuint _d_particleBuffer;
	GLuint _d_scale;
};
