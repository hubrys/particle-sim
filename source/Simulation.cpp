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

	glPointSize(4.f);
	_running = true;

	Timer timer;
	timer.reset();
	while(glfwWindowShouldClose(_window) == false && _running) 
	{
		glClear(GL_COLOR_BUFFER_BIT);
		_manager->tick(timer.lap());
		_manager->render();
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}

const char* Simulation::init()
{
	// initialize glfw and window
	if (glfwInit() == false) 
	{
		return "failed to initialize glfw";
	}

	int width = Config::instance()->getInt("windowWidth");
	int height = Config::instance()->getInt("windowHeight");

	_window = glfwCreateWindow(width, height, "Particle Simulation", nullptr, nullptr);
	if (_window == nullptr) 
	{
		return "failed to create window";
	}

	glfwMakeContextCurrent(_window);

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
	if (managerType == "cpu")
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

	glClearColor(1.0, 1.0, 1.0, 1.0);

	return nullptr;
}
