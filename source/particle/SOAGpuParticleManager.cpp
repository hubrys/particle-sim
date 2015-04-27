#include "SOAGpuParticleManager.h"

void launchSOACudaTick(float deltaTime, float particleMass, float mouseX, float mouseY, float mouseMass,
struct cudaGraphicsResource* positionsResource, struct cudaGraphicsResource* velocitiesResource, bool mouseOnly, int particleCount, float2 bounds, float friction);

const char* SOAGpuParticleManager::init()
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
    glm::vec2* positions = new glm::vec2[_particleCount];
    glm::vec2* velocities = new glm::vec2[_particleCount];

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
            positions[yI * dimX + xI] = glm::vec2(xStep * xI - xOffset, yStep * yI - yOffset);
            velocities[yI * dimX + xI] = glm::vec2(.00000001f);
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
    glUniformMatrix4fv(_d_projection, 1, GL_FALSE, glm::value_ptr(_projection));

    glGenVertexArrays(1, &_d_vao);
    glBindVertexArray(_d_vao);

    // Setup position buffer
    glGenBuffers(1, &_d_positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _d_positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, _particleCount * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _particleCount * sizeof(glm::vec2), positions);

    // Setup position buffer
    glGenBuffers(1, &_d_velocityBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _d_velocityBuffer);
    glBufferData(GL_ARRAY_BUFFER, _particleCount * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _particleCount * sizeof(glm::vec2), velocities);

    glBindBuffer(GL_ARRAY_BUFFER, _d_positionBuffer);
    GLuint d_vPosition = _program->getAttribute("vPosition");
    glEnableVertexAttribArray(d_vPosition);
    glVertexAttribPointer(d_vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _d_velocityBuffer);
    GLuint d_vVelocity = _program->getAttribute("vVelocity");
    glEnableVertexAttribArray(d_vVelocity);
    glVertexAttribPointer(d_vVelocity, 2, GL_FLOAT, GL_FALSE, 0,0);

    _particleDimX = dimX;
    _particleDimY = dimY;

    cudaError_t cudaResult = cudaGraphicsGLRegisterBuffer(&_cuda_positions, _d_positionBuffer, cudaGraphicsRegisterFlagsNone);
    if (cudaResult != cudaSuccess)
    {
        return "could not register position buffer";
    }

    cudaResult = cudaGraphicsGLRegisterBuffer(&_cuda_velocities, _d_velocityBuffer, cudaGraphicsRegisterFlagsNone);
    if (cudaResult != cudaSuccess)
    {
        return "could not register velocity buffer";
    }

    return nullptr;
}

void SOAGpuParticleManager::tick(float deltaTime, float particleMass, glm::vec2 mousePos, float mouseMass)
{
    launchSOACudaTick(deltaTime,
                   particleMass,
                   mousePos.x, mousePos.y,
                   mouseMass,
                   _cuda_positions,
                   _cuda_velocities,
                   _mouseOnly,
                   _particleDimX * _particleDimY,
                   make_float2(_bounds.x, _bounds.y),
                   _friction);
}

void SOAGpuParticleManager::render(float deltaTime)
{
    glBindVertexArray(_d_vao);
    glUniform1f(_d_deltaTime, deltaTime);
    glDrawArrays(GL_POINTS, 0, _particleCount);
}