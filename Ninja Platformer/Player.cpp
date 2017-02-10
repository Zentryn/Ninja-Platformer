#include "Player.h"
#include <Bengine/ResourceManager.h>
#include <SDL/SDL.h>

Player::Player()
{
}

Player::~Player()
{
}

void Player::init(b2World* world, const glm::vec2& position, const glm::vec2& drawDims, const glm::vec2& collisionDims, Bengine::ColorRGBA8 color)
{
    m_texture = Bengine::ResourceManager::getTexture("Assets/blue_ninja.png");
    m_color = color;
    m_drawDims = drawDims;

    m_capsule.init(world, position, collisionDims, 1.0f, 0.3f, true);
}

void Player::update(Bengine::InputManager& inputManager)
{
    const float MAX_SPEED = 20.0f;

    b2Body* body = m_capsule.getBody();

    if (inputManager.isKeyDown(SDLK_a) || inputManager.isKeyDown(SDLK_LEFT)) {
        body->ApplyForceToCenter(b2Vec2(-325.0f, 0.0f), true);
    }
    else if (inputManager.isKeyDown(SDLK_d) || inputManager.isKeyDown(SDLK_RIGHT)) {
        body->ApplyForceToCenter(b2Vec2(325.0f, 0.0f), true);
    }
    else {
        // Apply damping
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.99f, body->GetLinearVelocity().y));
    }

    if (body->GetLinearVelocity().x < -MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
    }
    else if (body->GetLinearVelocity().x > MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
    }

    // Loop through all the contact points
    for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next) {
        b2Contact* c = ce->contact;
        if (c->IsTouching()) {
            b2WorldManifold manifold;
            c->GetWorldManifold(&manifold);

            // Check if points are below
            bool below = false;
            for (size_t i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < body->GetPosition().y - m_capsule.getDimensions().y / 2.0f + 0.01f) {
                    // It's touching the player's feet
                    below = true;
                    break;
                }
            }

            if (below) {
                // We can jump
                if (inputManager.isKeyPressed(SDLK_w) || inputManager.isKeyPressed(SDLK_SPACE) || inputManager.isKeyPressed(SDLK_UP)) {
                    body->ApplyLinearImpulse(b2Vec2(0.0f, 60.0f), b2Vec2(0.0f, 0.0f), true);
                }
                break;
            }
        }
    }
}

void Player::draw(Bengine::SpriteBatch& spriteBatch)
{
    glm::vec4 destRect(
        m_capsule.getBody()->GetPosition().x - m_drawDims.x / 2.0f,
        m_capsule.getBody()->GetPosition().y - m_capsule.getDimensions().y / 2.0f,
        m_drawDims
    );

    glm::vec4 uvRect(0.0f, 0.0f, 0.1f, 0.5f);

    spriteBatch.draw(
        destRect,
        uvRect,
        m_texture.id,
        0.0f,
        m_color,
        m_capsule.getBody()->GetAngle()
    );
}


void Player::drawDebug(Bengine::DebugRenderer& debugRenderer)
{
    m_capsule.drawDebug(debugRenderer);
}
