#include "includes/testHttpResponse.hpp"

//por hora teste, mas depois possivelmente quando alterarmos coisas sera implementacao real
void bufferTestHttpResponse(ClientSocket& client)
{
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello, World!";

    client.appendToWriteBuffer(response, strlen(response));
}