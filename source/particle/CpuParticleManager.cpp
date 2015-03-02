#include "./CpuParticleManager.h"

CpuParticleManager::~CpuParticleManager()
{
    delete _program;
	delete[] _particles;
}

const char* CpuParticleManager::init()
{
    _gravConst = Config::instance()->getFloat("gravConst");
    _particleCount = Config::instance()->getInt("particleCount");
    int dim = std::sqrt(_particleCount);

    // Initialize particle array
    _particleCount = dim * dim;
    _particles = new CpuParticle[_particleCount];

    float width = Config::instance()->getFloat("windowWidth");
    float height = Config::instance()->getFloat("windowHeight");

    float xStep = width / dim;
    float yStep = height / dim;

    float xOffset = width / 2.f;
    float yOffset = height / 2.f;

    float xScale = 2 / width;
    float yScale = 2 / height;

    _scale = xScale < yScale ? xScale : yScale;

    for (int yI = 0; yI < dim; yI++) 
    {
        for (int xI = 0; xI < dim; xI++)
        {
            _particles[yI * dim + xI] = CpuParticle(glm::vec2(xStep * xI - xOffset, yStep * yI - yOffset),
                                                    glm::vec2(0));
        }
    }

    // Setup OpenGL values
    std::string assetRoot = Config::instance()->getString("assetRoot");
    _program = Program::createProgram(assetRoot + "shaders/simulation.vert",
                                      assetRoot + "shaders/simulation.frag");
    if (_program == nullptr)
    {
        return "CpuParticleManager: could not create program";
    }
    _program->setActive();

    _d_scale = _program->getUniform("scale");

    glGenVertexArrays(1, &_d_vao);
    glBindVertexArray(_d_vao);


    // Setup particle buffer
    glGenBuffers(1, &_d_particleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    glBufferData(GL_ARRAY_BUFFER, _particleCount * sizeof(CpuParticle), nullptr, GL_DYNAMIC_DRAW);

    GLuint d_vPosition = _program->getAttribute("vPosition");
    printf("position of vposition: %d\n", d_vPosition);
    glEnableVertexAttribArray(d_vPosition);
    glVertexAttribPointer(d_vPosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindVertexArray(0);

    return nullptr;
}

void CpuParticleManager::tick(float deltaTime)
{
    for (int particleI = 0; particleI < _particleCount; particleI++)
    {
        CpuParticle* particle = &_particles[particleI];

        for (int otherParticleI = 0; otherParticleI < _particleCount; otherParticleI++)
        {
            if (particleI == otherParticleI)
            {
                continue;
            }
            
            glm::vec2 diff = _particles[otherParticleI].position /*glm::vec2(0, 0)*/ - particle->position;
            float distance = glm::length(diff);
            float force = _gravConst / (distance * distance);
            particle->velocity += force * glm::normalize(diff);
        }
    }

    for (int particleI = 0; particleI < _particleCount; particleI++)
    {
        _particles[particleI].position += _particles[particleI].velocity * deltaTime;
    }
}

void CpuParticleManager::render()
{
    glUniform1f(_d_scale, _scale);
    // transfer data to GPU 
    glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _particleCount * sizeof(CpuParticle), _particles);

    glBindVertexArray(_d_vao);
    glDrawArrays(GL_POINTS, 0, _particleCount);
}