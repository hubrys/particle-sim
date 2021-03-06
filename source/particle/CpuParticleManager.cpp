#include "./CpuParticleManager.h"

CpuParticleManager::~CpuParticleManager()
{
    _program->destroy();
    glDeleteBuffers(1, &_d_particleBuffer);

    delete _program;
    delete[] _particles;
}

const char* CpuParticleManager::init()
{
    _gravConst = Config::instance()->getFloat("gravConst", 10);
    _particleCount = Config::instance()->getInt("particleCount", 100);
    _minCalcDistance = Config::instance()->getFloat("minCalcDistance", 1);
    _mouseOnly = Config::instance()->getInt("mouseOnly", 0);
    _friction = Config::instance()->getFloat("friction", 0);

    int dimX = Config::instance()->getInt("particleCountX", 32);
    int dimY = Config::instance()->getInt("particleCountY", 32);

    // Initialize particle array
    _particleCount =  dimX * dimY;
    _particles = new CpuParticle[_particleCount];

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
            _particles[yI * dimX + xI] = CpuParticle(glm::vec2(xStep * xI - xOffset, yStep * yI - yOffset),
                                                    glm::vec2(.00000001f));
            /*_particles[yI * dimX + xI] = CpuParticle(glm::vec2(xI * yI * .000000001f),
                                                     glm::vec2(.00000001f));*/
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

    glGenVertexArrays(1, &_d_vao);
    glBindVertexArray(_d_vao);

    // Setup particle buffer
    glGenBuffers(1, &_d_particleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    glBufferData(GL_ARRAY_BUFFER, _particleCount * sizeof(CpuParticle), nullptr, GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint d_vPosition = _program->getAttribute("vPosition");
    glEnableVertexAttribArray(d_vPosition);
    glVertexAttribPointer(d_vPosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    GLuint d_vVelocity = _program->getAttribute("vVelocity");
    glEnableVertexAttribArray(d_vVelocity);
    glVertexAttribPointer(d_vVelocity, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const void*)(2 * sizeof(GLfloat)));    

    glBindVertexArray(0);
    return nullptr;
}

void CpuParticleManager::tick(float deltaTime, float particleMass, glm::vec2 mousePosition, float mouseMass)
{
    CpuParticle mouseParticle = CpuParticle(mousePosition, glm::vec2(0));

    for (int particleI = 0; particleI < _particleCount; particleI++)
    {
        CpuParticle* particle = &_particles[particleI];
        glm::vec2 totalForce = calculateMouseForce(_particles[particleI], mousePosition, mouseMass) * particleMass;
        if (_mouseOnly == false)
        {
            for (int otherParticleI = 0; otherParticleI < _particleCount; otherParticleI++)
            {
                if (particleI == otherParticleI)
                {
                    continue;
                }
                totalForce += calculateForce(*particle, _particles[otherParticleI]) * particleMass * particleMass;

            }
        }
        particle->velocity += totalForce * deltaTime;
    }

    applyVelocity(deltaTime);
}

glm::vec2 CpuParticleManager::calculateForce(CpuParticle& lhs, CpuParticle& rhs)
{
    glm::vec2 diff = rhs.position - lhs.position;
    float distance = glm::length(diff) + _minCalcDistance;
    float magnitude = _gravConst / (distance * distance);
    return magnitude * glm::normalize(diff);   
}

glm::vec2 CpuParticleManager::calculateMouseForce(CpuParticle& lhs, glm::vec2& mousePos, float mouseMass)
{
    glm::vec2 diff = mousePos - lhs.position;
    float distance = glm::length(diff) + _minCalcDistance;
    float magnitude = _gravConst * mouseMass / (distance);//* distance);
    return magnitude * glm::normalize(diff);   
}

void CpuParticleManager::applyVelocity(float deltaTime)
{
    for (int particleI = 0; particleI < _particleCount; particleI++)
    {
        glm::vec2 velocity = _particles[particleI].velocity;
        _particles[particleI].position += velocity * deltaTime;
     
        glm::vec2 position = _particles[particleI].position;
        if (_bounds.x - glm::abs(position.x) < 0 && // if we are past the bounds
            position.x * velocity.x > 0) { // if velocity would send us farther past bounds
            velocity.x = -velocity.x;
        }
        if (_bounds.y - glm::abs(position.y) < 0 && // if we are past the bounds
            position.y * velocity.y > 0) { // if velocity would send us farther past bounds
            velocity.y = -velocity.y;
        }
        _particles[particleI].velocity = velocity * (1 - _friction * deltaTime);
    }
}

void CpuParticleManager::render(float deltaTime)
{
    glUniform1f(_d_deltaTime, deltaTime);
    glUniformMatrix4fv(_d_projection, 1, GL_FALSE, glm::value_ptr(_projection));
    // transfer data to GPU 
    glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _particleCount * sizeof(CpuParticle), _particles);

    glBindVertexArray(_d_vao);
    glDrawArrays(GL_POINTS, 0, _particleCount);
}