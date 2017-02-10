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

    void update(Bengine::InputManager& inputManager);
    void draw(Bengine::SpriteBatch& spriteBatch);
    void drawDebug(Bengine::DebugRenderer& debugRenderer);

    const Capsule& getCapsule() const { return m_capsule; }

private:
    glm::vec2 m_drawDims;
    Bengine::TileSheet m_texture;
    Bengine::ColorRGBA8 m_color;
    Capsule m_capsule;
    PlayerMoveState m_moveState = PlayerMoveState::STANDING;
    float m_animationTime = 0.0f;
    bool m_onGround = false;
    bool m_isPunching = false;
    int m_direction = 1; // 1 or -1
};

