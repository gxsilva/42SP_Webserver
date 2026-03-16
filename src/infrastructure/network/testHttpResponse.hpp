#ifndef TESTHTTPRESPONSE_HPP
#define TESTHTTPRESPONSE_HPP

#include "../../domain/entities/HttpResponse.hpp"
#include "clientSocket.hpp"

void bufferTestHttpResponse(ClientSocket& client, const HttpResponse& response);

#endif
