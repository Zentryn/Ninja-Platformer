#pragma once
#include <vector>

namespace Bengine {

    struct DirEntry {
        std::string path;
        bool isDirectory;
    };

	class IOManager
	{
	public:
		static bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
        static bool readFileToBuffer(std::string filePath, std::string& buffer);

        // Gets all directory entries in the directory specified by path and stores it in rvEntries
        // Returns false if path is not a directory
        static bool getDirectoryEntries(const char* path, std::vector<DirEntry>& rvEntries);
        // Creates a directory, returns false if couldn't make it
        static bool makeDirectory(const char* path);
	};
}