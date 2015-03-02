#include "./Simulation.h"

#include <chrono>
#include "./Timer.h"

Simulation::~Simulation()
{
	delete _manager;
	glfwTerminate();
}

void Simulation::execute(const std::string& root)
{
	Config config = Config::fromFile(root + "config.txt");

	const char* result = init(root, config);

	if (result != nullptr) 
	{
		printf("%s\n", result);
		return;
	}

	using namespace std::chrono;

	glPointSize(2.f);
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

const char* Simulation::init(const std::string& assetRoot, const Config& config)
{
	// initialize glfw and window
	if (glfwInit() == false) 
	{
		return "failed to initialize glfw";
	}

	int width = config.getInt("windowWidth");
	int height = config.getInt("windowHeight");

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

	_manager = new CpuParticleManager();
	const char* result = _manager->init(assetRoot, 100, glm::vec3(100, 100, 0));
	if (result != nullptr)
	{
		return result;
	}

	glClearColor(1.0, 1.0, 1.0, 1.0);

	return nullptr;
}
