#include "TextureCache.h"
#include "BengineErrors.h"
#include "ImageLoader.h"
#include <iostream>

namespace Bengine {

	TextureCache::TextureCache()
	{
	}


	TextureCache::~TextureCache()
	{
	}


	GLTexture TextureCache::getTexture(std::string texturePath)
	{
		// Look up the texture and check if it's in the map 
		auto mit = _textureMap.find(texturePath);

		// Check if it's not in the map
		if (mit == _textureMap.end()) {
			// Load the texture
			GLTexture newTexture = ImageLoader::loadPNG(texturePath);

			// Insert it into the map
			_textureMap.insert(make_pair(texturePath, newTexture));

			std::cout << "Loaded texture!\n";
			return newTexture;
		}

		//std::cout << "Loaded cached texture!\n";
		// Return the texture
		return mit->second;
	}

}