*Este projeto foi criado como parte do curriculo da 42 por lsilva-x, ailbezer, fleite-j*

# Webserv

## Descrição

`webserv` e um servidor HTTP/1.1 implementado do zero em **C++98** como projeto da 42.

O objetivo principal e reproduzir o comportamento essencial de um servidor web real (inspirado no NGINX), com foco em:

- I/O nao bloqueante orientado a eventos (epoll)
- Parsing e validacao de configuracao
- Parsing e validacao de requisicoes HTTP
- Suporte a metodos HTTP principais (`GET`, `POST`, `DELETE`)
- Execucao de CGI
- Servir arquivos estaticos, paginas de erro e roteamento por `location`

Arquiteturalmente, o projeto segue separacao por camadas (dominio, aplicacao, infraestrutura e interfaces), com classes pequenas e responsabilidades bem definidas.


### Funcionalidades Implementadas

- Servidor orientado a eventos com `epoll` e I/O nao bloqueante
- Parsing completo de configuracao (`Lexer -> Parser -> AST -> ConfigBuilder`)
- Parsing/validacao incremental de requests HTTP por conexao
- Fluxo funcional de `GET`, `POST` basico e `DELETE` basico
- CGI integrado ao event loop com timeout e tratamento de resposta
- Suporte a multiplos listeners no mesmo processo


## Instruções

### Requisitos

- Linux
- `c++` com suporte a C++98
- `make`

Ferramentas opcionais para qualidade de codigo:

- `clang-format`
- `clang-tidy`
- `bear`

### Compilacao

```bash
make
```

Isso gera o binario `./webserv` na raiz do repositorio.

### Execucao

```bash
./webserv config.conf
```

Depois de iniciar, voce pode testar em outro terminal:

```bash
curl -i http://localhost:8080/
curl -i http://localhost:8080/listener/
curl -i http://localhost:8080/cgi-bin/hello.py
```

### Testes

Executar todos os testes disponiveis:

```bash
make test
```

Executar/alvo por suite:

```bash
make test_http_request
make test_http_validation
make test_parse_validate_usecase
make test_cgi
```

### Limpeza e rebuild

```bash
make clean
make fclean
make re
```

### Onde encontrar mais informacoes

- Contexto do projeto: `doc/webserver_context.txt`
- Documento base da 42: `subject.txt` e `doc/subject.txt`
- Status e fluxo de desenvolvimento: `PROJECT_STATUS.md` e `PROJECT_FLOW.md`
- Diagnostico tecnico detalhado (progresso, gaps e proximos passos): `PROJECT_STATUS.md`

## Recursos

### Referencias tecnicas

- RFC 7230 (HTTP/1.1 Message Syntax and Routing): https://datatracker.ietf.org/doc/html/rfc7230
- RFC 7231 (HTTP/1.1 Semantics and Content): https://datatracker.ietf.org/doc/html/rfc7231
- RFC 3875 (CGI/1.1): https://datatracker.ietf.org/doc/html/rfc3875
- NGINX documentation: https://nginx.org/en/docs/
- `epoll(7)`: https://man7.org/linux/man-pages/man7/epoll.7.html

### Uso de IA no projeto

A IA foi usada como ferramenta de apoio tecnico, sem substituir o entendimento da equipe sobre o codigo. Principais usos:

- Revisao de clareza arquitetural (separacao de responsabilidades entre camadas)
- Apoio na redacao e melhoria de documentacao tecnica
- Sugestoes para casos de teste e validacao de cenarios HTTP/CGI
- Aceleracao de refatoracoes localizadas (nomes, organizacao e pequenas simplificacoes)

Partes com apoio mais direto de IA:

- Documentacao (`README.md` e ajustes de guias auxiliares)
- Revisoes pontuais de codigo para legibilidade e manutencao

Partes implementadas e validadas manualmente pela equipe:

- Logica principal do servidor (rede, parser, validacoes e orquestracao de metodos)
- Integracao de CGI e comportamento de runtime
- Decisoes finais de arquitetura, comportamento HTTP e criterios de teste
