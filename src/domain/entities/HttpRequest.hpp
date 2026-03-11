#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <map>
#include <string>

class HttpRequest
{
	public:
		HttpRequest();
		~HttpRequest();

		const std::string&						  getMethod() const;
		const std::string&						  getUri() const;
		const std::string&						  getVersion() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const std::string&						  getBody() const;

		void		setMethod(const std::string& method);
		void		setUri(const std::string& uri);
		void		setVersion(const std::string& version);
		void		setHeader(const std::string& name, const std::string& value);
		void		setBody(const std::string& body);
		std::string getHeader(const std::string& name) const;

	private:
		std::string						   _method;
		std::string						   _uri;
		std::string						   _version;
		std::map<std::string, std::string> _headers;
		std::string						   _body;

		std::string normalizeHeaderName(const std::string& name) const;
};

#endif // HTTPREQUEST_HPP
