#include "testHttpResponse.hpp"

void bufferTestHttpResponse(ClientSocket& client, const HttpResponse& response)
{
	std::string raw = response.serialize();
	client.appendToWriteBuffer(raw.c_str(), raw.size());
}
