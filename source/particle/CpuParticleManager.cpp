#include "./CpuParticleManager.h"

CpuParticleManager::~CpuParticleManager()
{
    delete _program;
	delete[] _particles;
}

const char* CpuParticleManager::init(int dim, glm::vec3 bounds, bool threeDimensional)
{
    // Initialize particle array
    _particleCount = dim * dim;
	_particles = new CpuParticle[_particleCount];

    float xOffset = 2.f / dim;
    float yOffset = 2.f / dim;

    for (int yI = 0; yI < dim; yI++) 
    {
        for (int xI = 0; xI < dim; xI++)
        {
            _particles[yI * dim + xI] = CpuParticle(glm::vec2(xOffset * xI - 1.f, yOffset * yI - 1.f),
                                                    glm::vec2(0));
        }
    }

    // Setup OpenGL values
    _program = Program::createProgram("assets/shaders/simulation.vert",
                                      "assets/shaders/simulation.frag");
    if (_program == nullptr)
    {
        return "CpuParticleManager: could not create program";
    }
    _program->setActive();

    glGenVertexArrays(1, &_d_vao);
    glBindVertexArray(_d_vao);


    // Setup particle buffer
    glGenBuffers(1, &_d_particleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    glBufferData(GL_ARRAY_BUFFER, _particleCount * sizeof(CpuParticle), nullptr, GL_DYNAMIC_DRAW);

    GLuint d_vPosition = _program->getAttribute("vPosition");
    printf("position of vposition: %d\n", d_vPosition);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(d_vPosition, 3, GL_FLOAT, GL_FALSE, 2, 0);

    glBindVertexArray(0);

    return nullptr;
}

void CpuParticleManager::tick(long deltaTime)
{
    glBindBuffer(GL_ARRAY_BUFFER, _d_particleBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _particleCount * sizeof(CpuParticle), _particles);
}

void CpuParticleManager::render()
{
    glBindVertexArray(_d_vao);
    glDrawArrays(GL_POINTS, 0, _particleCount);
}