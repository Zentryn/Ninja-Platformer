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

        // Adds the offset to the position and scale
        void offsetPosition(const glm::vec2& offset) { _position += offset; _needsMatrixUpdate = true; }
        void offsetScale(float offset) { _scale += offset; if (_scale < 0.001f) _scale = 0.001f; _needsMatrixUpdate = true; }

		// Setters
		void setScale(float newScale) { _scale = newScale; _needsMatrixUpdate = true; }
		void setPosition(const glm::vec2& newPosition) { _position = newPosition; _needsMatrixUpdate = true; }

		// Getters
		float getScale() const { return _scale; }
		glm::vec2 getPosition() const { return _position; }
		glm::mat4 getCameraMatrix() const { return _cameraMatrix; }
		glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);
        float getAspectRatio() const { return (float)_screenWidth / (float)_screenHeight; }


	private:
		int _screenWidth, _screenHeight;
		bool _needsMatrixUpdate;
		float _scale;
		glm::vec2 _position;
		glm::mat4 _cameraMatrix;
		glm::mat4 _orthoMatrix;
	};

}