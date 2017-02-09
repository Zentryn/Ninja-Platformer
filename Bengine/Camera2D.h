#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Bengine {

	class Camera2D
	{
	public:
		Camera2D();
		~Camera2D();

		void init(int screenWidth, int screenHeight);
		void update();

		bool isBoxInView(const glm::vec2& position, const glm::vec2& dimensions);

		// Setters
		void setScale(float newScale) { _scale = newScale; _needsMatrixUpdate = true; }
		void setPosition(const glm::vec2& newPosition) { _position = newPosition; _needsMatrixUpdate = true; }

		// Getters
		float getScale() const { return _scale; }
		glm::vec2 getPosition() const { return _position; }
		glm::mat4 getCameraMatrix() const { return _cameraMatrix; }
		glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);


	private:
		int _screenWidth, _screenHeight;
		bool _needsMatrixUpdate;
		float _scale;
		glm::vec2 _position;
		glm::mat4 _cameraMatrix;
		glm::mat4 _orthoMatrix;
	};

}