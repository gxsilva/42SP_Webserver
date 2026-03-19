#ifndef DIRECTORYREADER_HPP
#define DIRECTORYREADER_HPP

#include <string>
#include <vector>

struct DirectoryReader
{
		static bool isDirectory(const std::string& path);
		static bool readDirectory(const std::string& path, std::vector< std::string >& entries);
};

#endif
