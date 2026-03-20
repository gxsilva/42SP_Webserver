#include "HttpRequestFramer.hpp"

#include <cctype>
#include <cstdlib>

namespace
{
	bool findHeaderEnd(const std::string& data, size_t& headerEndPos, size_t& delimiterLen)
	{
		headerEndPos = data.find("\r\n\r\n");
		if (headerEndPos != std::string::npos)
		{
			delimiterLen = 4;
			return true;
		}

		headerEndPos = data.find("\n\n");
		if (headerEndPos != std::string::npos)
		{
			delimiterLen = 2;
			return true;
		}

		return false;
	}

	std::string toUpperAscii(const std::string& s)
	{
		std::string out = s;
		for (size_t i = 0; i < out.size(); ++i)
			out[i] = static_cast< char >(std::toupper(static_cast< unsigned char >(out[i])));
		return out;
	}

	bool parseHeaderLineContentLength(const std::string& headerLine, size_t& outContentLength)
	{
		std::string::size_type colon = headerLine.find(':');
		if (colon == std::string::npos)
			return false;

		std::string name = toUpperAscii(headerLine.substr(0, colon));
		if (name != "CONTENT-LENGTH")
			return false;

		std::string			   value = headerLine.substr(colon + 1);
		std::string::size_type first = value.find_first_not_of(" \t");
		if (first == std::string::npos)
			return true;
		std::string::size_type last = value.find_last_not_of(" \t");
		value						= value.substr(first, last - first + 1);

		char* end	 = NULL;
		long  parsed = std::strtol(value.c_str(), &end, 10);
		if (*end != '\0' || parsed < 0)
			return true;

		outContentLength = static_cast< size_t >(parsed);
		return true;
	}

	bool tryExtractContentLength(const std::string& headerBlock, bool& hasContentLength, size_t& contentLength)
	{
		hasContentLength = false;
		contentLength	 = 0;

		size_t start = 0;
		while (start < headerBlock.size())
		{
			size_t lineEnd	 = headerBlock.find("\r\n", start);
			size_t lineDelim = 2;
			if (lineEnd == std::string::npos)
			{
				lineEnd	  = headerBlock.find('\n', start);
				lineDelim = 1;
			}
			if (lineEnd == std::string::npos)
				lineEnd = headerBlock.size();

			std::string line = headerBlock.substr(start, lineEnd - start);
			if (line.empty())
				break;

			size_t parsedLength = 0;
			if (parseHeaderLineContentLength(line, parsedLength))
			{
				hasContentLength = true;
				contentLength	 = parsedLength;
				break;
			}

			start = lineEnd + lineDelim;
		}

		return true;
	}
} // namespace

bool HttpRequestFramer::popCompleteRequestFromBuffer(std::string& buffer, std::string& rawRequest)
{
	if (buffer.empty())
		return false;

	size_t headerEndPos = 0;
	size_t delimiterLen = 0;
	if (!findHeaderEnd(buffer, headerEndPos, delimiterLen))
		return false;

	size_t		headerEnd	= headerEndPos + delimiterLen;
	std::string headerBlock = buffer.substr(0, headerEnd);

	bool   hasContentLength = false;
	size_t contentLength	= 0;
	tryExtractContentLength(headerBlock, hasContentLength, contentLength);

	size_t requiredSize = headerEnd;
	if (hasContentLength)
		requiredSize += contentLength;

	if (buffer.size() < requiredSize)
		return false;

	rawRequest = buffer.substr(0, requiredSize);
	buffer.erase(0, requiredSize);
	return true;
}
