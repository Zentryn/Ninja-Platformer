#pragma once

#include <Bengine/Vertex.h>
#include <Bengine/SpriteBatch.h>
#include <glm/glm.hpp>
#include <iostream>

class FlashLight {
public:
	void draw(Bengine::SpriteBatch& spriteBatch, const int windowWidth, const int windowHeight) {
		glm::vec4 destRect(
			(-windowWidth / 2.0f + position.x - size) / 32.0f,
			(-windowHeight / 2.0f + position.y - size) / 32.0f,
			size * 2.0f / 32.0f, size * 2.0f / 32.0f
		);

		//glm::vec4 destRect(-m_window->getScreenWidth() / 2.0f + lightPosition.x - lightLength / 2.0f,
		//				   -m_window->getScreenHeight() / 2.0f + lightPosition.y - lightLength / 2.0f, lightLength, lightLength);

		spriteBatch.draw(
			destRect,
			glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f),
			0,
			0.0f,
			color
		);

		//std::cout << "dir: " << direction.x << ", diry: " << direction.y << "\n";
	}

	Bengine::ColorRGBA8 color;
	glm::vec2 position;
	glm::vec2 direction;
	float size;
};