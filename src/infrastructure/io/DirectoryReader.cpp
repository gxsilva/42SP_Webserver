#include "DirectoryReader.hpp"

#include <dirent.h>
#include <sys/stat.h>

bool DirectoryReader::isDirectory(const std::string& path)
{
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return (false);
	return (S_ISDIR(st.st_mode));
}

bool DirectoryReader::readDirectory(const std::string& path,
									std::vector<std::string>& entries)
{
	DIR* dir = opendir(path.c_str());
	if (dir == NULL)
		return (false);

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name(entry->d_name);
		if (name == ".")
			continue;
		entries.push_back(name);
	}
	closedir(dir);
	return (true);
}
