#include "IOManager.h"
#include <fstream>
#include <filesystem>

// Namespace alias
namespace fs = std::tr2::sys;

namespace Bengine {

bool IOManager::readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer)
{
	// Load the file
	std::ifstream file(filePath, std::ios::binary);
	if (file.fail()) {
		perror(filePath.c_str());
		return false;
	}

	// Seek to the end
	file.seekg(0, std::ios::end);

	// Get the file size
	int fileSize = file.tellg();

	// Seek back to the beginning
	file.seekg(0, std::ios::beg);

	// Remove the header from the file size now that we're in the beginning
	fileSize -= file.tellg();

	// Resize the buffer to the size of the file
	buffer.resize(fileSize);

	// Read the file
	file.read((char *)&(buffer[0]), fileSize);

	// Close the file
	file.close();

	return true;
}

bool IOManager::readFileToBuffer(std::string filePath, std::string& buffer)
{
    // Load the file
    std::ifstream file(filePath, std::ios::binary);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    // Seek to the end
    file.seekg(0, std::ios::end);

    // Get the file size
    int fileSize = file.tellg();

    // Seek back to the beginning
    file.seekg(0, std::ios::beg);

    // Remove the header from the file size now that we're in the beginning
    fileSize -= file.tellg();

    // Resize the buffer to the size of the file
    buffer.resize(fileSize);

    // Read the file
    file.read((char *)&(buffer[0]), fileSize);

    // Close the file
    file.close();

    return true;
}

bool IOManager::getDirectoryEntries(const char* path, std::vector<DirEntry>& rvEntries)
{
    auto dpath = fs::path(path);

    // Must be a directory
    if (!fs::is_directory(dpath)) return false;

    for (auto it = fs::directory_iterator(dpath); it != fs::directory_iterator(); ++it) {
        rvEntries.emplace_back();
        rvEntries.back().path = it->path().string();
        if (is_directory(it->path())) {
            rvEntries.back().isDirectory = true;
        }
        else {
            rvEntries.back().isDirectory = false;
        }
    }

    return true;
}

bool IOManager::makeDirectory(const char* path)
{
    return fs::create_directory(fs::path(path));
}

}