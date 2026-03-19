#ifndef MIMETYPE_HPP
#define MIMETYPE_HPP

#include <map>
#include <string>

class MimeType
{
	public:
		explicit MimeType(const std::string& filePath);
		~MimeType();

		const std::string& getValue() const;

	private:
		std::string _type;

		static std::map< std::string, std::string > initMimeMap();
		std::string									extractExtension(const std::string& path) const;
		std::string									lookupMime(const std::string& extension) const;
};

#endif
