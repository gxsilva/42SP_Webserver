#include "MimeType.hpp"

#include <cctype>

MimeType::MimeType(const std::string& filePath)
{
	std::string ext = extractExtension(filePath);
	_type = lookupMime(ext);
}

MimeType::~MimeType() {}

const std::string& MimeType::getValue() const { return (_type); }

std::string MimeType::extractExtension(const std::string& path) const
{
	size_t dot = path.rfind('.');
	if (dot == std::string::npos)
		return ("");

	std::string ext = path.substr(dot);
	for (size_t i = 0; i < ext.size(); ++i)
		ext[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(ext[i])));
	return (ext);
}

std::map<std::string, std::string> MimeType::initMimeMap() //tipo de conteúdo
{
	std::map<std::string, std::string> m;
	m[".html"] = "text/html";
	m[".htm"] = "text/html";
	m[".css"] = "text/css";
	m[".js"] = "application/javascript";
	m[".json"] = "application/json";
	m[".png"] = "image/png";
	m[".jpg"] = "image/jpeg";
	m[".jpeg"] = "image/jpeg";
	m[".gif"] = "image/gif";
	m[".svg"] = "image/svg+xml";
	m[".ico"] = "image/x-icon";
	m[".txt"] = "text/plain";
	m[".xml"] = "application/xml";
	m[".pdf"] = "application/pdf";
	m[".mp4"] = "video/mp4";
	m[".webm"] = "video/webm";
	m[".mp3"] = "audio/mpeg";
	m[".woff"] = "font/woff";
	m[".woff2"] = "font/woff2";
	return (m);
}

std::string MimeType::lookupMime(const std::string& extension) const
{
	static std::map<std::string, std::string> mimeMap = initMimeMap();

	std::map<std::string, std::string>::const_iterator it = mimeMap.find(extension);
	if (it != mimeMap.end())
		return (it->second);
	return ("application/octet-stream");
}
