#include "includes/testHttpResponse.hpp"
#include <cstring>

bool sendTestHttpResponse(ClientSocket& client)
{
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello, World!";

    ssize_t sent = client.sendData(response, strlen(response));
    
    return (sent > 0);
}