#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <string>
#include <vector>

namespace Bengine {

	enum WindowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4 };

	class Window
	{
	public:
		Window();
		~Window();

		int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags, bool vSync);
		void swapBuffer();

		int getScreenWidth() { return _screenWidth; }
		int getScreenHeight() { return _screenHeight; }
		std::vector<WindowFlags> getFlags() { return _currentFlags; }
	private:
		std::vector<WindowFlags> _currentFlags;
		SDL_Window* _sdlWindow;
		int _screenWidth;
		int _screenHeight;
	};

}