#pragma once

#include <Bengine/Vertex.h>
#include <Bengine/SpriteBatch.h>
#include <glm/glm.hpp>

class Light {
public:
    void draw(Bengine::SpriteBatch& spriteBatch) {
        glm::vec4 destRect(
            position.x - size / 2.0f,
            position.y - size / 2.0f,
            size, size
        );

        spriteBatch.draw(
            destRect,
            glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f),
            0,
            0.0f,
            color,
            0.0f
        );
    }

    Bengine::ColorRGBA8 color;
    glm::vec2 position;
    float size;
};