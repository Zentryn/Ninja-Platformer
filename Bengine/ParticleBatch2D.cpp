#include "ParticleBatch2D.h"

namespace Bengine {

ParticleBatch2D::ParticleBatch2D()
{
    // Empty
}


ParticleBatch2D::~ParticleBatch2D()
{
	delete[] m_particles;
}


void ParticleBatch2D::init(int maxParticles,
                           float decayRate,
                           GLTexture texture,
                           std::function<void(Particle2D&, float)> updateFunc /* = defaultParticleUpdate */)
{
	m_maxParticles = maxParticles;
	m_particles = new Particle2D[maxParticles];
    m_decayRate = decayRate;
    m_texture = texture;
    m_updateFunc = updateFunc;
}


void ParticleBatch2D::update(float deltaTime)
{
    for (unsigned i = 0; i < m_maxParticles; i++) {
        // Check if particle is active
        if (m_particles[i].life > 0.0f) {
            m_updateFunc(m_particles[i], deltaTime);
            m_particles[i].life -= m_decayRate * deltaTime;
        }
    }
}


void ParticleBatch2D::draw(SpriteBatch* spriteBatch)
{
    const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    for (unsigned i = 0; i < m_maxParticles; i++) {
        auto& p = m_particles[i];

        // Check if particle is active
        if (p.life > 0.0f) {
            glm::vec4 destRect(p.position.x, p.position.y, p.size, p.size);

            spriteBatch->draw(destRect, uvRect, m_texture.id, 0.0f, p.color);
        }
    }
}


void ParticleBatch2D::addParticle(const glm::vec2& position,
								  const glm::vec2& velocity,
								  const ColorRGBA8& color,
                                  float size)
{
    // Find a free particle
    int particleIndex = findFreeParticle();

    auto& p = m_particles[particleIndex];

    // Initialize the new particle
    p.life = 1.0f;
    p.position = position;
    p.velocity = velocity;
    p.color = color;
    p.size = size;
}


int ParticleBatch2D::findFreeParticle()
{
    for (unsigned i = m_lastFreeParticle; i < m_maxParticles; i++) {
        if (m_particles[i].life <= 0.0f) {
            m_lastFreeParticle = i;
            return i;
        }
    }

    for (unsigned i = 0; i < m_lastFreeParticle; i++) {
        if (m_particles[i].life <= 0.0f) {
            m_lastFreeParticle = i;
            return i;
        }
    }

    // If no particles are free
    return 0;
}

}