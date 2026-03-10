#include "../statusCodeResponse.hpp"
#include "../ErrPage.hpp"
#include <iostream>

int main()
{
    StatusCodeResponse helper;

    std::cout << "--- Testando Gerador de Paginas de Erro ---" << std::endl;

    // Teste 404
    std::cout << "\n[ Gerando HTML para 404 ]\n" << std::endl;
    std::string html404 = ErrorPageGenerator::generate(NOT_FOUND, helper);
    std::cout << html404 << std::endl;

    // Teste 500
    std::cout << "\n[ Gerando HTML para 500 ]\n" << std::endl;
    std::string html500 = ErrorPageGenerator::generate(INTERNAL_SERVER_ERROR, helper);
    std::cout << html500 << std::endl;

    return (0);
}