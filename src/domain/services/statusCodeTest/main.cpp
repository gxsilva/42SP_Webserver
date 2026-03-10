#include "../statusCodeResponse.hpp"
#include <iostream>

int main()
{
    StatusCodeResponse helper;

    std::cout << "--- Testando Status Codes ---" << std::endl;

    std::cout << "Input 200: " << helper.statusReturn(OK) << std::endl;

    std::cout << "Input 404: " << helper.statusReturn(NOT_FOUND) << std::endl;

    std::cout << "Input 500: " << helper.statusReturn(INTERNAL_SERVER_ERROR) << std::endl;

    std::cout << "Input 999: " << helper.statusReturn(static_cast<HttpStatusCode>(999)) << std::endl;

    return (0);
}