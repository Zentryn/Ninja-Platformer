#pragma once

#include "Capsule.h"
#include <Bengine/SpriteBatch.h>
#include <Bengine/TileSheet.h>
#include <Bengine/InputManager.h>   
#include <Bengine/DebugRenderer.h>

enum class PlayerMoveState {STANDING, RUNNING, PUNCHING, IN_AIR};

class Player
{
public:
    Player();
    ~Player();

    void init(
        b2World* world,
        const glm::vec2& position,
        const glm::vec2& drawDims,
        const glm::vec2& collisionDims,
        Bengine::ColorRGBA8 color
    );

    void destroy(b2World* world);
    void update(Bengine::InputManager& inputManager);
    void draw(Bengine::SpriteBatch& spriteBatch);
    void drawDebug(Bengine::DebugRenderer& debugRenderer);

    const Capsule& getCapsule() const { return m_capsule; }
    const glm::vec2& getDrawDims() const { return m_drawDims; }
    const glm::vec2& getCollisionDims() const { return m_collisionDims; }
    const Bengine::ColorRGBA8 getColor() const { return m_color; }
    glm::vec2 getPosition() const { return glm::vec2(m_capsule.getBody()->GetPosition().x, m_capsule.getBody()->GetPosition().y); }

private:
    glm::vec2 m_drawDims;
    glm::vec2 m_collisionDims;
    Bengine::TileSheet m_texture;
    Bengine::ColorRGBA8 m_color;
    Capsule m_capsule;
    PlayerMoveState m_moveState = PlayerMoveState::STANDING;
    float m_animationTime = 0.0f;
    bool m_onGround = false;
    bool m_isPunching = false;
    int m_direction = 1; // 1 or -1
};

