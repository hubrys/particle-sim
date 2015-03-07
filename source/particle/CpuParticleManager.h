#pragma once

#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	virtual void tick(float deltaTime, glm::vec2 mousePos, float mouseMass);
	virtual void render(float deltaTime);

protected:
	int _particleCount;
	CpuParticle* _particles;

	bool _mouseOnly;
	float _gravConst;
	float _scale;
	float _minCalcDistance;
	float _mouseMass;
	float _friction;

	glm::vec2 _bounds;

	Program* _program;
	GLuint _d_vao;
	GLuint _d_particleBuffer;
	GLuint _d_projection;
	GLuint _d_deltaTime;
	glm::mat4 _projection;

	glm::vec2 calculateForce(CpuParticle& lhs, CpuParticle& rhs);
	glm::vec2 calculateMouseForce(CpuParticle& lhs, glm::vec2& mousePos, float mouseMass);
	void applyVelocity(float deltaTime);
};
