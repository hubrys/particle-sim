#include "./GpuParticleManager.h"

void launchCudaTick(float deltaTime, float particleMass, float mouseX, float mouseY, float mouseMass,
struct cudaGraphicsResource* particleResource, bool sourceBufferFirst, bool mouseOnly, int particleCount, float2 bounds, float friction);


const char* GpuParticleManager::init()
{
    _gravConst = Config::instance()->getFloat("gravConst", 10);
    _particleCount = Config::instance()->getInt("particleCount", 100);
    _minCalcDistance = Config::instance()->getFloat("minCalcDistance", 1);
    _mouseOnly = Config::instance()->getInt("mouseOnly", 0);
    _friction = Config::instance()->getFloat("friction", 0);

    int dimX = Config::instance()->getInt("particleCountX", 32);
    int dimY = Config::instance()->getInt("particleCountY", 32);

    // Initialize particle array
    _particleCount = dimX * dimY;
    glm::vec4* particles = new glm::vec4[_particleCount];

    float worldScale = Config::instance()->getFloat("worldScale");
    float ratio = Config::instance()->getFloat("windowWidth") /
        Config::instance()->getFloat("windowHeight");
    float width = worldScale * ratio;
    float height = worldScale;

    float xStep = width / dimX;
    float yStep = height / dimY;

    float xOffset = width / 2.f;
    float yOffset = height / 2.f;

    _bounds = glm::vec2(xOffset, yOffset);
    _projection = glm::ortho(-xOffset, xOffset, -yOffset, yOffset, 0.f, 20000.f);

    for (int yI = 0; yI < dimY; yI++)
    {
        for (int xI = 0; xI < dimX; xI++)
        {
            particles[yI * dimX + xI] = glm::vec4(xStep * xI - xOffset, yStep * yI - yOffset, .001f, .001f);
        }
    }

    // Setup OpenGL values
    std::string fragType = Config::instance()->getString("colorBy");
    std::string assetRoot = Config::instance()->getString("assetRoot");
    _program = Program::createProgram(assetRoot + "shaders/simulation.vert",
                                      assetRoot + "shaders/colorBy" + fragType + ".frag");
    if (_program == nullptr)
    {
        return "CpuParticleManager: could not create program";
    }
    _program->setActive();

    _d_projection = _program->getUniform("Projection");
    _d_deltaTime = _program->getUniform("DeltaTime");

    glGenBuffers(1, &_d_buffer);
    glGenVertexArrays(1, &_d_vao);
    glBindVertexArray(_d_vao);

    // Setup particle buffer
    glBindBuffer(GL_ARRAY_BUFFER, _d_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * _particleCount * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

    GLuint d_vPosition = _program->getAttribute("vPosition");
    glEnableVertexAttribArray(d_vPosition);
    glVertexAttribPointer(d_vPosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    GLuint d_vVelocity = _program->getAttribute("vVelocity");
    glEnableVertexAttribArray(d_vVelocity);
    glVertexAttribPointer(d_vVelocity, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const void*) (2 * sizeof(GLfloat)));

    cudaError_t cudaResult = cudaGraphicsGLRegisterBuffer(&_cuda_particles, _d_buffer, cudaGraphicsRegisterFlagsNone);
    if (cudaResult != cudaSuccess)
    {
        return "could not register buffer";
    }

    _particleDimX = dimX;
    _particleDimY = dimY;

    int numBytes = _particleCount * sizeof(glm::vec4);
    glBindBuffer(GL_ARRAY_BUFFER, _d_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, particles);
    glBufferSubData(GL_ARRAY_BUFFER, numBytes, numBytes, particles);
    glUniformMatrix4fv(_d_projection, 1, GL_FALSE, glm::value_ptr(_projection));

    _sourceBufferFirst = true;

    return nullptr;
}

void GpuParticleManager::tick(float deltaTime, float particleMass, glm::vec2 mousePos, float mouseMass)
{
    launchCudaTick(deltaTime, 
    particleMass, 
    mousePos.x, mousePos.y, 
    mouseMass, 
    _cuda_particles,
    _sourceBufferFirst,
    _mouseOnly,
    _particleDimX * _particleDimY,
    make_float2(_bounds.x, _bounds.y),
    _friction);

    _sourceBufferFirst = !_sourceBufferFirst;
}

void GpuParticleManager::render(float deltaTime)
{
    glBindVertexArray(_d_vao);
    glUniform1f(_d_deltaTime, deltaTime);
    glDrawArrays(GL_POINTS, _sourceBufferFirst ? 0 : _particleCount, _particleCount);
}