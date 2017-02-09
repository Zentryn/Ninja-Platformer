#include "Window.h"
#include "BengineErrors.h"

namespace Bengine {

Window::Window()
{
}


Window::~Window()
{
}


int Window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags, bool vSync)
{
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	Uint32 flags = SDL_WINDOW_OPENGL;

	// Set the flags
	if (currentFlags & INVISIBLE) {
		flags |= SDL_WINDOW_HIDDEN;
		_currentFlags.push_back(WindowFlags::INVISIBLE);
	}
	if (currentFlags & FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		_currentFlags.push_back(WindowFlags::FULLSCREEN);
	}
	if (currentFlags & BORDERLESS) {
		flags |= SDL_WINDOW_BORDERLESS;
		_currentFlags.push_back(WindowFlags::BORDERLESS);
	}

	// Creates the window
	_sdlWindow = SDL_CreateWindow(
		windowName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_screenWidth,
		_screenHeight,
		flags
	);

	if (_sdlWindow == nullptr) {
		fatalError("SDL Window could not be created.");
	}

	// Sets up the OpenGL Context
	SDL_GLContext glContext = SDL_GL_CreateContext(_sdlWindow);

	if (glContext == nullptr) {
		fatalError("SDL GL Context could not be created.");
	}

	// Initialize glew
	GLenum error = glewInit();

	if (error != GLEW_OK) {
		fatalError("Could not initialize glew.");
	}

	// Prints the version of OpenGL
	std::printf("***   OpenGL version: %s   ***\n", glGetString(GL_VERSION));

	// Sets the background color that the window clears to
	glClearColor(0.65f, 0.65f, 0.65f, 1.0f);

	// Set Vertical Synchronization (V-SYNC). 1 = ON, 0 = OFF
	SDL_GL_SetSwapInterval(static_cast<int>(vSync));

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 0;
}


void Window::swapBuffer()
{
	SDL_GL_SwapWindow(_sdlWindow);
}

}