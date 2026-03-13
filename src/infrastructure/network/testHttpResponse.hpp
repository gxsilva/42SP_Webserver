#ifndef TESTHTTPRESPONSE_HPP
#define TESTHTTPRESPONSE_HPP

#include "clientSocket.hpp"
#include "../../domain/entities/HttpResponse.hpp"

void bufferTestHttpResponse(ClientSocket& client, const HttpResponse& response);

#endif
