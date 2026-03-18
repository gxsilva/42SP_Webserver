#ifndef DIRECTORYLISTER_HPP
#define DIRECTORYLISTER_HPP

#include <string>
#include <vector>

class DirectoryLister
{
	public:
		DirectoryLister();
		~DirectoryLister();

		std::string generateHtml(const std::string& uri,
								 const std::vector<std::string>& entries) const;

	private:
		std::string escapeHtml(const std::string& text) const;
};

#endif
