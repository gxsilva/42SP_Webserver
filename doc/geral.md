# Webserv - Visao Geral Completa

Ultima atualizacao: 2026-03-19

## 1. Objetivo do projeto

O projeto webserv implementa um servidor HTTP/1.x do zero em C++98, com foco em:
- robustez
- separacao de responsabilidades
- IO nao bloqueante
- conformidade com os requisitos do subject da 42

A ideia central e transformar um arquivo de configuracao em comportamento real de servidor, recebendo conexoes de rede, interpretando requests HTTP, escolhendo uma estrategia de resposta (arquivo estatico, CGI, upload, delete, redirect), e retornando respostas HTTP validas.

## 2. Por que HTTP e importante

HTTP e o protocolo base da web. Sem ele, browser e servidor nao conseguem negociar:
- qual recurso o cliente quer
- como os dados devem ser transferidos
- qual foi o resultado da operacao (status code)

No contexto deste projeto, entender HTTP importa por 3 motivos:

1. Semantica de metodos
- GET deve buscar representacoes de recursos
- POST envia dados para processamento/criacao
- DELETE remove recurso

2. Contrato de interoperabilidade
- um servidor precisa gerar respostas padrao para qualquer cliente HTTP
- headers, status line, body e regras de validacao precisam ser previsiveis

3. Seguranca e robustez
- validacao de URI, headers e versao evita comportamento indefinido
- tratamento correto de codigos de erro evita inconsistencias no cliente

## 3. Papel do servidor no projeto

O servidor e o orquestrador runtime. Ele conecta:
- camada de configuracao
- camada de rede
- camada de roteamento/metodos HTTP
- pipeline CGI

Na pratica, ele:
- abre sockets de escuta
- coloca tudo no mesmo epoll
- aceita clientes
- despacha leitura/escrita por evento
- integra pipes CGI no mesmo loop

Esse design evita bloqueio global e permite concorrencia de clientes sem thread por conexao.

## 4. Multiporta: importancia e impacto

Multiporta significa que o mesmo processo do servidor consegue escutar mais de uma porta ao mesmo tempo (exemplo: 8091 e 8092), com um unico loop de eventos.

Importancia:
- atende requisito obrigatorio do subject
- permite separar ambientes/logicas por porta
- aproxima o comportamento de servidores reais (vhosts, segmentos de servico)

No estado atual:
- o servidor ja sobe multiplos listen sockets no mesmo epoll
- cada socket de servidor gera accepts no mesmo event loop
- requests em portas diferentes respondem corretamente

Ponto ideal ainda pendente:
- associar cada conexao ao ServerBlock do listener de origem
- hoje o bootstrap multiporta funciona, mas regras por porta ainda podem ficar acopladas ao primeiro config em alguns fluxos do dispatcher

## 5. Arquitetura geral (DDD + separacao por camadas)

A organizacao segue uma separacao consistente:

- interfaces
  - ponto de entrada do programa
- application
  - orquestracao de casos de uso e fluxo runtime
- domain
  - entidades, regras de negocio HTTP/config
- infrastructure
  - IO real, sockets, arquivos, parser de request bruto, logging

Objetivo da separacao:
- trocar implementacao de infraestrutura com minimo impacto nas regras de dominio
- manter classes pequenas com responsabilidade clara

## 6. Fluxo end-to-end atual (o que acontece do inicio ao fim)

### 6.1 Bootstrap
1. main recebe caminho do config
2. BuildServerConfig roda pipeline de validacao e parse
3. ConfigBuilder transforma AST em HttpBlock e ServerBlock(s)
4. Server inicia com lista de servidores configurados

### 6.2 Inicializacao de rede
1. para cada ServerBlock, cria ServerSocket
2. bind + listen
3. registra fd de listen no EpollManager
4. entra no loop run()

### 6.3 Loop de eventos
1. epoll_wait retorna eventos prontos
2. se fd e de listen, aceita novos clientes
3. se fd e de cliente e EPOLLIN, le request
4. parse + validate da request
5. despacha para CGI ou metodo HTTP (GET/POST/DELETE)
6. serializa resposta e envia em EPOLLOUT
7. desconecta cliente ao final

### 6.4 Caminho CGI
1. request com extensao cgi elegivel
2. CgiOrchestrator inicia processo e registra pipes no epoll
3. loop principal trata leitura/escrita dos pipes por evento
4. ao finalizar, monta HttpResponse e volta para socket do cliente

## 7. Como cada arquivo principal se comporta

Abaixo esta o mapa dos arquivos mais criticos do que ja foi coberto e implementado.

### 7.1 Entrada e bootstrap

- src/interfaces/cli/main.cpp
  - Entry point do binario
  - Carrega config via BuildServerConfig
  - Extrai lista de ServerBlock e inicia Server
  - Faz log de inicializacao e trata erros de startup

- src/application/use_cases/BuildServerConfig.cpp
  - Orquestra validacao/leitura/tokenizacao/parse/analise semantica/build
  - Retorna ConfigResult (ok/erro) para o main

### 7.2 Configuracao (arquivo .conf para objetos de runtime)

- src/domain/services/config/Lexer.cpp
  - Tokeniza o arquivo de configuracao

- src/domain/services/config/Parser.cpp
  - Converte tokens em AST

- src/infrastructure/io/config/SemanticAnalyzer.cpp
  - Valida regras semanticas (cardinalidade, contexto, dependencias)

- src/domain/services/config/ConfigBuilder.cpp
  - Converte AST em HttpBlock/ServerBlock/LocationBlock
  - Parseia listen, root, index, allow_methods, return, etc
  - Agrega multiplos server blocks em HttpBlock.servers

- src/domain/entities/server/HttpBlock.hpp
  - Estrutura de alto nivel da config
  - Mantem server de compatibilidade e vetor servers para multiporta

- src/domain/entities/server/ServerBlock.hpp
  - Config por servidor: host, port, root, index, limites e locations

- src/domain/entities/server/LocationBlock.hpp
  - Config por rota: path, metodos permitidos, autoindex, redirect, error pages

### 7.3 Runtime de rede

- src/application/network/server.hpp
- src/application/network/server.cpp
  - Nucleo do loop de eventos
  - Inicializa varios ServerSocket
  - Registra todos no epoll
  - Encaminha eventos para ConnectionManager e CgiOrchestrator

- src/infrastructure/network/serverSocket.hpp
- src/infrastructure/network/serverSocket.cpp
  - Wrapper de socket de escuta
  - setNonBlocking, bind, listen, accept

- src/application/network/epollManager.hpp
- src/application/network/epollManager.cpp
  - Wrapper de epoll_create/epoll_ctl/epoll_wait
  - Add/modify/remove fd

- src/application/network/connectionManager.hpp
- src/application/network/connectionManager.cpp
  - Gerencia sockets de cliente
  - Le request, parseia, valida
  - Decide entre CGI e HTTP method handlers
  - Faz fila de resposta e envio

- src/infrastructure/network/clientSocket.hpp
- src/infrastructure/network/clientSocket.cpp
  - Buffer de escrita e leitura do socket do cliente
  - Flush de resposta em EPOLLOUT

### 7.4 HTTP request/response

- src/infrastructure/io/request/HttpRequestParser.cpp
  - Converte raw string em objeto HttpRequest

- src/domain/entities/HttpRequest.cpp
  - Estrutura da request normalizada

- src/domain/services/request/HttpRequestValidator.cpp
  - Regras de validacao HTTP
  - Metodos permitidos, URI, Host, Content-Length, versao HTTP/1.0 e HTTP/1.1

- src/domain/entities/HttpResponse.cpp
  - Estrutura da resposta
  - serializacao para formato HTTP

- src/infrastructure/network/testHttpResponse.cpp
  - Ajuda a bufferizar resposta no cliente

### 7.5 Metodos HTTP

- src/application/methods/HttpMethodOrchestrator.cpp
  - Facade de despacho por metodo

- src/application/methods/GetRequestHandler.cpp
  - Serve arquivo estatico, index, autoindex, redirect

- src/application/methods/PostRequestHandler.cpp
  - Upload basico e validacoes basicas

- src/application/methods/DeleteRequestHandler.cpp
  - Remove arquivo com protecoes basicas

- src/domain/services/DirectoryLister.cpp
  - Gera listagem de diretorio para autoindex

### 7.6 CGI

- src/application/CGI/CgiOrchestrator.cpp
  - Coordena sessoes CGI concorrentes
  - Integra pipes no epoll

- src/application/CGI/CgiHandler.cpp
  - Orquestra execucao CGI para uma request

- src/domain/CGI/CgiEnvironment.cpp
  - Monta variaveis de ambiente CGI

- src/domain/CGI/CgiResponse.cpp
  - Parse do output do CGI para status/headers/body

- src/infrastructure/CGI/CgiProcessExecutor.cpp
  - fork + execve + pipes non-blocking + timeout

## 8. O que ja esta solido hoje

- Build principal e testes locais passando
- Pipeline de config completo (lexer -> parser -> AST -> semantic -> builder)
- Loop de eventos unico para rede + CGI
- Metodos GET/POST/DELETE basicos
- Redirect por return em location
- Suporte HTTP/1.1 no validador
- Multiporta funcional no mesmo processo

## 9. O que e ideal concluir em seguida

### 9.1 Prioridade alta
1. Associacao conexao -> ServerBlock por listener
- garante regras corretas quando portas tiverem configs diferentes

2. client_max_body_size unificado
- usar valor da config no validador e handlers de forma consistente

3. error_page custom por config/location
- atualmente respostas de erro ainda nao usam toda capacidade de configuracao

### 9.2 Prioridade media
1. parser HTTP incremental
- suportar fragmentacao real de request em multiplos chunks

2. CGI totalmente guiado por config (cgi_path/cgi_ext)
- reduzir hardcode por extensao/interpreter

3. desacoplar dispatcher do ConnectionManager
- melhorar SRP e testabilidade

## 10. Relacao com o subject

Coberturas principais ja alinhadas:
- C++98
- epoll unico para IO
- IO orientado a readiness
- fork restrito a CGI
- GET/POST/DELETE
- multiporta

Pontos de conformidade que ainda pedem fechamento para entrega forte:
- paginas de erro padrao/custom bem conectadas no pipeline
- robustez extrema para nunca crashar sob falhas e fragmentacao
- consistencia completa de politicas por porta/location

## 11. Conclusao

Hoje o projeto ja funciona como um servidor HTTP nao bloqueante realista, com pipeline de configuracao serio, loop de eventos unificado e suporte multiporta validado.

O passo mais importante para maturidade arquitetural e funcional e fechar a selecao de config por listener e consolidar as politicas de validacao/resposta por contexto de servidor. Com isso, o comportamento fica coerente para cenarios multiporta de producao e mais fiel ao modelo esperado pelo subject.
