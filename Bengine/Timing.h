#pragma once

#include <SDL\SDL.h>
#include <iostream>

namespace Bengine {

class FPSLimiter {
public:
	FPSLimiter();

	void init(float maxFPS);

	void setMaxFPS(float maxFPS);

	void printFPS(bool print);

	void begin();

	// Will return the current FPS
	float end();

	float calculateFPS();
private:
	bool _print;
	float _maxFPS;
	float _fps;
	int _frames;
	float _frameTime;
	unsigned int _startTicks;
};

}