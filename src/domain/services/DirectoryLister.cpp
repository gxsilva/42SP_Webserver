#include "DirectoryLister.hpp"

#include <sstream>

DirectoryLister::DirectoryLister() {}
DirectoryLister::~DirectoryLister() {}

std::string DirectoryLister::escapeHtml(const std::string& text) const
{
	std::string result;
	for (size_t i = 0; i < text.size(); ++i)
	{
		if (text[i] == '<')
			result += "&lt;";
		else if (text[i] == '>')
			result += "&gt;";
		else if (text[i] == '&')
			result += "&amp;";
		else if (text[i] == '"')
			result += "&quot;";
		else
			result += text[i];
	}
	return (result);
}

std::string DirectoryLister::generateHtml(const std::string& uri, const std::vector< std::string >& entries) const
{
	std::ostringstream oss;
	oss << "<html>\r\n<head><title>Index of " << escapeHtml(uri) << "</title></head>\r\n"
		<< "<body>\r\n<h1>Index of " << escapeHtml(uri) << "</h1>\r\n"
		<< "<hr><pre>\r\n";

	for (size_t i = 0; i < entries.size(); ++i)
	{
		std::string name = entries[i];
		std::string href = uri;
		if (href.empty() || href[href.size() - 1] != '/')
			href += "/";
		href += name;

		oss << "<a href=\"" << escapeHtml(href) << "\">" << escapeHtml(name) << "</a>\r\n";
	}

	oss << "</pre><hr>\r\n"
		<< "<center>42_Webserver/1.0</center>\r\n"
		<< "</body>\r\n</html>\r\n";

	return (oss.str());
}
