#include "BengineErrors.h"
#include <iostream>
#include <SDL/SDL.h>
#include <cstdlib>

namespace Bengine {

	void fatalError(std::string errorString) {
		std::cout << "Fatal Error: " + errorString << std::endl;
		std::cout << "Enter any key to quit" << std::endl;

		int tmp;
		std::cin >> tmp;
		SDL_Quit();
		exit(69);
	}

}