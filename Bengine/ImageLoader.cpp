#include "ImageLoader.h"
#include "picoPNG.h"
#include "IOManager.h"
#include "BengineErrors.h"

namespace Bengine {

	GLTexture ImageLoader::loadPNG(std::string filePath)
	{
		// Create the texture with all values set to 0
		GLTexture texture = {};

		// Create the variables for the PNG generation
		std::vector<unsigned char> in;
		std::vector<unsigned char> out;
		unsigned long width, height;

		// Read the PNG file
		if (!IOManager::readFileToBuffer(filePath, in)) {
			fatalError("Failed to load PNG file " + filePath + " to buffer");
		}

		// Decode the PNG to our "in" variable.
		int errorCode = decodePNG(out, width, height, &in[0], in.size());

		if (errorCode != 0) {
			fatalError("decondePNG failed with error code " + std::to_string(errorCode));
		}

		// Generate 1 texture (our texture)
		glGenTextures(1, &(texture.id));

		// Bind our texture
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(out[0]));

		// Set the parameters for the texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// Generate the mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind our texture
		glBindTexture(GL_TEXTURE_2D, 0);

		// Set the width and height of the GLTexture
		texture.width = width;
		texture.height = height;

		return texture;
	}

}