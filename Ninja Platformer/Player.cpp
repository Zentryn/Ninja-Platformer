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
    Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Assets/blue_ninja.png");
    m_color = color;
    m_drawDims = drawDims;

    m_capsule.init(world, position, collisionDims, 1.0f, 0.3f, true);
    m_texture.init(texture, glm::ivec2(10, 2));
}

void Player::update(Bengine::InputManager& inputManager)
{
    const float MAX_SPEED = 20.0f;

    b2Body* body = m_capsule.getBody();

    if (inputManager.isKeyDown(SDLK_a) || inputManager.isKeyDown(SDLK_LEFT)) {
        body->ApplyForceToCenter(b2Vec2(-325.0f, 0.0f), true);
        m_direction = -1;
    }
    else if (inputManager.isKeyDown(SDLK_d) || inputManager.isKeyDown(SDLK_RIGHT)) {
        body->ApplyForceToCenter(b2Vec2(325.0f, 0.0f), true);
        m_direction = 1;
    }
    else {
        // Apply damping
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.99f, body->GetLinearVelocity().y));
    }

    // Limit the speed of the player
    if (body->GetLinearVelocity().x < -MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
    }
    else if (body->GetLinearVelocity().x > MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
    }

    // Punch
    if (inputManager.isKeyPressed(SDLK_SPACE)) {
        m_isPunching = true;
    }

    // Loop through all the contact points
    m_onGround = false;
    for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next) {
        b2Contact* c = ce->contact;
        if (c->IsTouching()) {
            b2WorldManifold manifold;
            c->GetWorldManifold(&manifold);

            // Check if points are below
            for (size_t i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < body->GetPosition().y - m_capsule.getDimensions().y / 2.0f + m_capsule.getDimensions().x / 2.0f + 0.01f) {
                    // It's touching the player's feet
                    m_onGround = true;
                    break;
                }
            }

            if (m_onGround) {
                // We can jump
                if (inputManager.isKeyPressed(SDLK_w) || inputManager.isKeyPressed(SDLK_UP)) {
                    body->ApplyLinearImpulse(b2Vec2(0.0f, 60.0f), b2Vec2(0.0f, 0.0f), true);
                }
                break;
            }
        }
    }
}

void Player::draw(Bengine::SpriteBatch& spriteBatch)
{
    b2Body* body = m_capsule.getBody();

    glm::vec4 destRect(
        body->GetPosition().x - m_drawDims.x / 2.0f,
        body->GetPosition().y - m_capsule.getDimensions().y / 2.0f,
        m_drawDims
    );

    int tileIndex;
    int numTiles;

    // Get velocity
    b2Vec2 velocity = body->GetLinearVelocity();

    float animationSpeed = 0.1f;
    // Calculate animation
    if (m_onGround) {
        if (m_isPunching) {
            // Punching on ground
            numTiles = 4;
            tileIndex = 1;
            if (m_moveState != PlayerMoveState::PUNCHING) {
                m_moveState = PlayerMoveState::PUNCHING;
                m_animationTime = 0.0f;
            }
        }
        else if (abs(velocity.x) > 1.0f && (velocity.x > 0 && m_direction > 0) || (velocity.x < 0 && m_direction < 0)) {
            // Running
            numTiles = 6;
            tileIndex = 10;
            animationSpeed = abs(velocity.x) * 0.005f;
            if (m_moveState != PlayerMoveState::RUNNING) {
                m_moveState = PlayerMoveState::RUNNING;
                m_animationTime = 0.0f;
            }
        }
        else {
            // Standing
            numTiles = 1;
            tileIndex = 0;
            m_moveState = PlayerMoveState::STANDING;
        }
    }
    else {
        // In the air
        if (m_isPunching) {
            // Kicking in the air
            animationSpeed *= 0.25f;
            numTiles = 1;
            tileIndex = 18;
            if (m_moveState != PlayerMoveState::PUNCHING) {
                m_moveState = PlayerMoveState::PUNCHING;
                m_animationTime = 0.0f;
            }
        }
        else if (abs(velocity.x) > 19.5f) {
            // Going fast in the air
            numTiles = 1;
            tileIndex = 10;
            m_moveState = PlayerMoveState::IN_AIR;
        }
        else if (velocity.y <= 0) {
            // Falling
            numTiles = 1;
            tileIndex = 17;
            m_moveState = PlayerMoveState::IN_AIR;
        }
        else {
            // Rising
            numTiles = 1;
            tileIndex = 16;
            m_moveState = PlayerMoveState::IN_AIR;
        }
    }

    // Increment animation time
    m_animationTime += animationSpeed;

    // Check for punch end
    if (m_animationTime > numTiles) m_isPunching = false;

    // Apply animation
    tileIndex += (int)m_animationTime % numTiles;

    // Get UV coordinates
    glm::vec4 uvRect = m_texture.getUV(tileIndex);

    // Check direction
    if (m_direction == -1) {
        uvRect.x += 1.0f / m_texture.dims.x;
        uvRect.z *= -1;
    }

    // Draw the sprite
    spriteBatch.draw(
        destRect,
        uvRect,
        m_texture.texture.id,
        0.0f,
        m_color,
        m_capsule.getBody()->GetAngle()
    );
}


void Player::drawDebug(Bengine::DebugRenderer& debugRenderer)
{
    m_capsule.drawDebug(debugRenderer);
}
