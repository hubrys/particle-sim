#define GLFW_INCLUDE_NONE
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Config.h"
#include "./particle/Particles.h"

class Simulation
{
public:
	~Simulation();
	bool execute();

private:
	GLFWwindow* _window;
	GLuint _points;
	IParticleManager* _manager = nullptr;

	glm::vec2 _windowDimens;
	glm::vec2 _mousePosition;
	float _worldScale;
	bool _useMouse;
	bool _restart;

	const char* init();
	const void reloadConfig();

	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void errorCallback(int error, const char* description);
	static void staticMouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};