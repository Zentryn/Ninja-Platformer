#include "Timing.h"

namespace Bengine {

FPSLimiter::FPSLimiter() : _startTicks(0), _frames(0), _print(false)
{
}


void FPSLimiter::init(float maxFPS)
{
	setMaxFPS(maxFPS);
}


void FPSLimiter::setMaxFPS(float maxFPS)
{
	_maxFPS = maxFPS;
}


void FPSLimiter::printFPS(bool print)
{
	_print = print;
}


void FPSLimiter::begin()
{
	_startTicks = SDL_GetTicks();

	if (_print) {
		if (_frames++ == 10) {
			std::cout << "FPS: " << _fps << std::endl;
			_frames = 0;
		}
	}
}


float FPSLimiter::end()
{
	calculateFPS();

	float frameTicks = SDL_GetTicks() - _startTicks;

	// Limit the FPS to the maxFPS
	if (_maxFPS >= 0) {
		if (1000.0f / _maxFPS > frameTicks) {
			SDL_Delay(1000.0f / _maxFPS - frameTicks);
		}
	}

	return _fps;
}


float FPSLimiter::calculateFPS()
{
	static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	static float previousTicks = SDL_GetTicks();
	float currentTicks;

	currentTicks = SDL_GetTicks();

	_frameTime = currentTicks - previousTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;

	previousTicks = currentTicks;

	int count;

	currentFrame++;
	if (currentFrame < NUM_SAMPLES) {
		count = currentFrame;
	}
	else {
		count = NUM_SAMPLES;
	}

	float frameTimeAverage = 0;
	for (int i = 0; i < count; i++) {
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;

	if (frameTimeAverage > 0) {
		_fps = 1000.0f / frameTimeAverage;
	}
	else {
		_fps = 60.0f;
	}

	return _fps;
}

}