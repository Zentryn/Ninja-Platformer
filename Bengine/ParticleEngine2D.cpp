#include "ParticleEngine2D.h"
#include "ParticleBatch2D.h"
#include "SpriteBatch.h"

namespace Bengine {

ParticleEngine2D::ParticleEngine2D()
{
    // Empty
}


ParticleEngine2D::~ParticleEngine2D()
{
    for (auto& batch : m_batches) {
        delete batch;
    }
}


void ParticleEngine2D::addParticleBatch(ParticleBatch2D* particleBatch)
{
    m_batches.push_back(particleBatch);
}


void ParticleEngine2D::update(float deltaTime)
{
    for (auto& batch : m_batches) {
        batch->update(deltaTime);
    }
}


void ParticleEngine2D::draw(SpriteBatch* spriteBatch)
{
    for (auto& batch : m_batches) {
        spriteBatch->begin();

        batch->draw(spriteBatch);

        spriteBatch->end();
        spriteBatch->renderBatch();
    }
}


}