#include "InputManager.h"

namespace Bengine {

InputManager::InputManager() : _mouseCoords(0.0f)
{
}


InputManager::~InputManager()
{
}


void InputManager::pressKey(unsigned int keyID)
{
	_keyMap[keyID] = true;
}


void InputManager::releaseKey(unsigned int keyID)
{
	_keyMap[keyID] = false;
}


bool InputManager::isKeyDown(unsigned int keyID)
{
	// Create iterator for the keymap
	auto it = _keyMap.find(keyID);

	// Check if the key is in the keymap
	if (it != _keyMap.end()) {
		return it->second;
	}
	else {
		false;
	}
}


bool InputManager::isKeyPressed(unsigned int keyID)
{
	return (!wasKeyDown(keyID) && isKeyDown(keyID));
}


bool InputManager::wasKeyDown(unsigned int keyID)
{
	return _previousKeyMap[keyID];
}


void InputManager::update()
{
	// Loop through keyMap and store the key states to the previous keyMap
	for (auto& it : _keyMap) {
		_previousKeyMap[it.first] = it.second;
	}
}


void InputManager::setMouseCoords(float x, float y)
{
	_mouseCoords.x = x;
	_mouseCoords.y = y;
}

}