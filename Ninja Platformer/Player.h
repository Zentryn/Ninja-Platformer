#pragma once

#include "Capsule.h"
#include <Bengine/SpriteBatch.h>
#include <Bengine/GLTexture.h>
#include <Bengine/InputManager.h>   
#include <Bengine/DebugRenderer.h>

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
    Bengine::GLTexture m_texture;
    Bengine::ColorRGBA8 m_color;
    Capsule m_capsule;
};

