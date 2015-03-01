#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Config.h"
#include "./particle/Particles.h"

class Simulation
{
public:
	~Simulation();
	void execute(const Config& config);

private:
	GLFWwindow* _window;
	GLuint _points;
	bool _running;
	IParticleManager* _manager;

	const char* init(const Config& config);

};