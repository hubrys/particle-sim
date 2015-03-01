#include "./Simulation.h"

Simulation::~Simulation()
{
	delete _manager;
	glfwTerminate();
}

void Simulation::execute(const Config& config)
{
	const char* result = init(config);

	if (result != nullptr) 
	{
		printf("%s\n", result);
		return;
	}

	_running = true;
	while(glfwWindowShouldClose(_window) == false && _running) 
	{
		_manager->tick(100);
		_manager->render();
		glfwPollEvents();
	}
}

const char* Simulation::init(const Config& config)
{
	// initialize glfw and window
	if (glfwInit() == false) 
	{
		return "failed to initialize glfw";
	}

	int width = config.getInt("windowWidth");
	int height = config.getInt("windowHeight");

	_window = glfwCreateWindow(width, height, "Paritcle Simulation", nullptr, nullptr);
	if (_window == nullptr) 
	{
		return "failed to create window";
	}

	glfwMakeContextCurrent(_window);

	// initialize glew
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		return reinterpret_cast<const char*>(glewGetErrorString(err));
	}
	printf("glsl version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	_manager = new CpuParticleManager();
	const char* result = _manager->init(100, glm::vec3(100, 100, 0));
	if (result != nullptr)
	{
		return result;
	}

	glClearColor(1.0, 1.0, 1.0, 1.0);

	return nullptr;
}
