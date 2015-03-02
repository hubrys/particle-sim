#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Config.h"
#include "./particle/Particles.h"

class Simulation
{
public:
	~Simulation();
	void execute(const std::string& assetPath);

private:
	GLFWwindow* _window;
	GLuint _points;
	bool _running;
	IParticleManager* _manager;

	const char* init(const std::string& assetRoot, const Config& config);
};