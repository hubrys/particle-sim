#include "./Simulation.h"

#include <chrono>
#include "./Timer.h"

Simulation::~Simulation()
{
    delete _manager;
    glfwTerminate();
}

bool Simulation::execute()
{
    _restart = false;
    const char* result = init();

    if (result != nullptr)
    {
        printf("%s\n", result);
        return false;
    }

    using namespace std::chrono;

    glPointSize(Config::instance()->getFloat("pointSize", 1.f));

    float particleMass = Config::instance()->getFloat("particleMass", 1.f);
    float mouseMass = Config::instance()->getFloat("mouseMass");
    float timeStep = Config::instance()->getFloat("timeStep", .1f);

    long frameCount = 0;
    Timer fpsTimer;
    Timer timer;
    timer.reset();
    float accumulation = 0;

    _useMouse = false;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    if (Config::instance()->getInt("useFixedTimeStep"))
    {
        fpsTimer.reset();
        while (glfwWindowShouldClose(_window) == false && _restart == false)
        {
            // process inputs
            glfwPollEvents();

            // update
            accumulation += timer.lap();
            while (accumulation >= timeStep)
            {
                _manager->tick(timeStep, particleMass, _mousePosition, _useMouse ? mouseMass : 0);
                accumulation -= timeStep;
            }

            // render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _manager->render(accumulation);
            glfwSwapBuffers(_window);
            frameCount++;
        }
    }
    else
    {
        fpsTimer.reset();
        while (glfwWindowShouldClose(_window) == false && _restart == false)
        {
            // process inputs
            glfwPollEvents();

            // update
            _manager->tick(timer.lap(), particleMass, _mousePosition, _useMouse ? mouseMass : 0);

            // render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _manager->render(0.f);
            glfwSwapBuffers(_window);
            frameCount++;
        }
    }
    float totalTime = fpsTimer.lap();
    float fps = frameCount / totalTime;
    printf("FrameCount: %d, TotalTime: %f, FPS: %f\n", frameCount, totalTime, fps);

    return _restart;
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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int width = Config::instance()->getInt("windowWidth");
    int height = Config::instance()->getInt("windowHeight");
    _windowDimens = glm::vec2(width, height);
    _worldScale = Config::instance()->getFloat("worldScale");

    GLFWmonitor* monitor = Config::instance()->getInt("useFullscreen") ? glfwGetPrimaryMonitor() : nullptr;
    _window = glfwCreateWindow(width, height, "Particle Simulation", monitor, nullptr);
    if (_window == nullptr)
    {
        return "failed to create window";
    }
    glfwSetWindowUserPointer(_window, this);

    //glfwSwapInterval(0);
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
    printf("manager type: %s\n", managerType.c_str());
    if (managerType == "cpuPair")
    {
        _manager = new CpuParticleManager();
    }
    else if (managerType == "cpuStepped")
    {
        _manager = new SteppedCpuParticleManager();
    }
    else if (managerType == "gpuPair")
    {
        _manager = new GpuParticleManager();
    }
    else if (managerType == "gpuSOA")
    {
        _manager = new SOAGpuParticleManager();
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
    _mousePosition.x = (2 * float(xpos) - _windowDimens.x) / (2 * _windowDimens.y);
    _mousePosition.y = -(float(ypos) / _windowDimens.y - .5f);
    _mousePosition *= _worldScale;
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

    IF_KEY_PRESSED(GLFW_KEY_R)
    {
        _restart = true;
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
