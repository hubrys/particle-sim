#include "./Simulation.h"

#include <chrono>
#include "./Timer.h"

Simulation::~Simulation()
{
	delete _manager;
	glfwTerminate();
}

void Simulation::execute()
{
	const char* result = init();

	if (result != nullptr) 
	{
		printf("%s\n", result);
		return;
	}

	using namespace std::chrono;

	glPointSize(Config::instance()->getFloat("pointSize", 1.f));
	_running = true;

	float mouseMass = Config::instance()->getFloat("mouseMass");

	Timer timer;
	timer.reset();
	while(glfwWindowShouldClose(_window) == false && _running) 
	{
		glClear(GL_COLOR_BUFFER_BIT);
		_manager->tick(timer.lap(), _mousePosition, _useMouse ? mouseMass : 0);
		_manager->render();
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}

const char* Simulation::init()
{
	// initialize glfw and window
	glfwSetErrorCallback(errorCallback);	
	if (glfwInit() == false) 
	{
		return "failed to initialize glfw";
	}
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Config::instance()->getInt("glVersionMajor", 1));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Config::instance()->getInt("glVersionMinor", 0));
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	int width = Config::instance()->getInt("windowWidth");
	int height = Config::instance()->getInt("windowHeight");
    _windowDimens = glm::vec2(width, height);

	_window = glfwCreateWindow(width, height, "Particle Simulation", nullptr, nullptr);
	if (_window == nullptr) 
	{
		return "failed to create window";
	}
	glfwSetWindowUserPointer(_window, this);

	glfwMakeContextCurrent(_window);
	glfwSetCursorPosCallback(_window, staticMouseCallback);
	glfwSetMouseButtonCallback(_window, staticMouseButtonCallback);
    glfwSetKeyCallback(_window, staticKeyCallback);
    glfwSetInputMode(_window, GLFW_STICKY_KEYS, 1);

	// initialize glew
	glewExperimental = GL_TRUE; 
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		return reinterpret_cast<const char*>(glewGetErrorString(err));
	}
	printf("glsl version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	float gravConst = Config::instance()->getFloat("gravConst", 1);

	std::string managerType = Config::instance()->getString("particleManager", "cpu");
	if (managerType == "cpuPair")
	{
		_manager = new CpuParticleManager();
	} 
	else if (managerType == "cpuStepped") 
	{
		_manager = new SteppedCpuParticleManager();
	}

	const char* result = _manager->init();
	if (result != nullptr)
	{
		return result;
	}

	glClearColor(0.0, 0.0, 0.0, 1.0);

	return nullptr;
}

#define IF_KEY_PRESSED(input) if ((key == input) && action == GLFW_PRESS)
#define IF_KEY_RELEASED(input) if ((key == input) && action == GLFW_RELEASE)

void Simulation::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	_mousePosition = glm::vec2(float(xpos) - _windowDimens.x / 2, 
							   _windowDimens.y / 2 - float(ypos));
}

void Simulation::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void Simulation::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	IF_KEY_PRESSED(GLFW_KEY_Q)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	IF_KEY_PRESSED(GLFW_KEY_SPACE)
	{
		_useMouse = !_useMouse;
	}
}

// Static input helpers
void Simulation::errorCallback(int error, const char* description)
{
	printf("GLFW ERROR: code: %d, %s\n", error, description);
}

void Simulation::staticMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	static_cast<Simulation*>(glfwGetWindowUserPointer(window))->mouseCallback(window, xpos, ypos);
}

void Simulation::staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	static_cast<Simulation*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(window, button, action, mods);
}

void Simulation::staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static_cast<Simulation*>(glfwGetWindowUserPointer(window))->keyCallback(window, key, scancode, action, mods);
}
