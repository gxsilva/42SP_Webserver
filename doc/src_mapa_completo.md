# Mapa Completo do `src/` - Webserv

Atualizado em: 2026-03-19

Este documento foca em como o projeto funciona hoje, com visao de fluxo geral + papel de cada arquivo `.cpp/.hpp` em `src/`.

## 1) Onde o fluxo geral comeca

- Ponto de entrada: `src/interfaces/cli/main.cpp` (funcao `main`).
- A execucao do servidor passa para: `src/application/network/server.cpp` em `Server::run()`.

Sequencia de boot (alto nivel):

1. `main.cpp` registra sinais e inicializa logger.
2. `BuildServerConfig::execute(...)` carrega e valida o arquivo `.conf`.
3. `Server` recebe os `ServerBlock` e cria sockets/listeners.
4. `Server::run()` entra no loop de epoll.
5. `ConnectionManager` aceita clientes, le/parsa request, chama metodo/CGI, e envia response.

## 2) Fluxo de request (do socket ate a resposta)

1. Evento no listener -> `acceptNewClient(...)`.
2. Evento `EPOLLIN` do cliente -> `handleClientRead(...)`.
3. Framing/parsing -> `HttpRequestFramer` + `HttpRequestParser`.
4. Validacao -> `ParseAndValidateHttpRequestUseCase` + `HttpRequestValidator`.
5. Roteamento:
6. Se CGI -> `CgiRouteResolver` + `CgiOrchestrator` + `CgiProcessExecutor`.
7. Se nao CGI -> `HttpMethodOrchestrator` (GET/POST/DELETE handlers).
8. Montagem de `HttpResponse`.
9. Evento `EPOLLOUT` -> `handleClientWrite(...)` envia buffer ao cliente.

## 3) Mapa por camadas

- `interfaces/`: inicio do programa e portas de integracao.
- `application/`: orquestracao de fluxo, casos de uso, handlers HTTP e CGI.
- `domain/`: entidades, regras, servicos de negocio e value objects.
- `infrastructure/`: sockets, parser/framer, filesystem, logging, execucao CGI.

## 4) Ordem recomendada de estudo (incremental)

1. `src/interfaces/cli/main.cpp`
2. `src/application/network/server.hpp`
3. `src/application/network/server.cpp`
4. `src/application/network/epollManager.hpp`
5. `src/application/network/connectionManager.hpp`
6. `src/application/network/connectionManager.cpp`
7. `src/application/use_cases/ParseAndValidateHttpRequestUseCase.hpp`
8. `src/infrastructure/io/request/HttpRequestFramer.hpp`
9. `src/infrastructure/io/request/HttpRequestParser.hpp`
10. `src/application/methods/HttpMethodOrchestrator.hpp`
11. `src/application/methods/GetRequestHandler.hpp`
12. `src/application/methods/PostRequestHandler.hpp`
13. `src/application/methods/DeleteRequestHandler.hpp`
14. `src/application/CGI/CgiOrchestrator.hpp`
15. `src/application/CGI/CgiHandler.hpp`
16. `src/infrastructure/CGI/CgiProcessExecutor.hpp`
17. `src/application/use_cases/BuildServerConfig.hpp`
18. `src/domain/entities/server/HttpBlock.hpp`
19. `src/domain/entities/server/ServerBlock.hpp`
20. `src/domain/entities/server/LocationBlock.hpp`

## 5) Inventario completo do `src/` (arquivo -> papel)

Observacao: onde algo estiver parcial, a descricao representa o papel no fluxo geral.

### 5.1 `src/interfaces/`

- `src/interfaces/cli/main.cpp` -> entrypoint do processo; bootstrap de config, logger e server.
- `src/interfaces/port/InitCgiGateway.hpp` -> contrato/porta para inicializacao de execucao CGI.

### 5.2 `src/application/ports/`

- `src/application/ports/ILogger.hpp` -> interface de logging para desacoplar aplicacao da implementacao concreta.

### 5.3 `src/application/network/`

- `src/application/network/server.hpp` -> definicao da classe `Server` e API do loop principal.
- `src/application/network/server.cpp` -> construcao do runtime (epoll, listeners, managers) e loop de eventos.
- `src/application/network/serverHandlers.cpp` -> apoio para tratamento de eventos e delegacao interna do `Server`.
- `src/application/network/epollManager.hpp` -> contrato da camada de gerenciamento de epoll.
- `src/application/network/epollManager.cpp` -> wrapper de `epoll_*` (register, modify, wait, event access).
- `src/application/network/connectionManager.hpp` -> contrato de gerenciamento de clientes, requests e respostas.
- `src/application/network/connectionManager.cpp` -> hub da pipeline HTTP (accept/read/parse/validate/route/write).

### 5.4 `src/application/use_cases/`

- `src/application/use_cases/BuildServerConfig.hpp` -> caso de uso para compilar arquivo de config em entidades de dominio.
- `src/application/use_cases/BuildServerConfig.cpp` -> fluxo completo de leitura/tokenizacao/parser/semantica/build do config.
- `src/application/use_cases/ParseAndValidateHttpRequestUseCase.hpp` -> caso de uso da pipeline request raw -> request valida.
- `src/application/use_cases/ParseAndValidateHttpRequestUseCase.cpp` -> integra parser HTTP e validador de regra HTTP.
- `src/application/use_cases/CompileSourceFile.hpp` -> utilitario de compilacao auxiliar (suporte).
- `src/application/use_cases/CompileSourceFile.cpp` -> implementacao utilitaria de compilacao (suporte).

### 5.5 `src/application/methods/`

- `src/application/methods/HttpMethodOrchestrator.hpp` -> contrato de despacho por metodo HTTP.
- `src/application/methods/HttpMethodOrchestrator.cpp` -> decide handler para GET/POST/DELETE.
- `src/application/methods/GetRequestHandler.hpp` -> API de atendimento de GET.
- `src/application/methods/GetRequestHandler.cpp` -> serve arquivo estatico/listagem conforme config.
- `src/application/methods/PostRequestHandler.hpp` -> API de atendimento de POST.
- `src/application/methods/PostRequestHandler.cpp` -> grava upload/body em destino configurado.
- `src/application/methods/DeleteRequestHandler.hpp` -> API de atendimento de DELETE.
- `src/application/methods/DeleteRequestHandler.cpp` -> remove recurso alvo e responde status.
- `src/application/methods/HttpResponseBuilders.hpp` -> helpers de montagem de `HttpResponse` (suporte).

### 5.6 `src/application/CGI/`

- `src/application/CGI/CgiOrchestrator.hpp` -> contrato de orquestracao de sessoes CGI concorrentes.
- `src/application/CGI/CgiOrchestrator.cpp` -> ciclo de vida CGI (start, monitoramento fd, coleta e despacho).
- `src/application/CGI/CgiHandler.hpp` -> API de execucao CGI por request.
- `src/application/CGI/CgiHandler.cpp` -> monta ambiente, chama executor e transforma saida em resposta HTTP.
- `src/application/CGI/CgiRouteResolver.hpp` -> API para decidir se request e CGI e resolver rota/script.
- `src/application/CGI/CgiRouteResolver.cpp` -> regra de resolucao CGI com base em URI/extensao/config.

### 5.7 `src/domain/entities/`

- `src/domain/entities/HttpRequest.hpp` -> entidade de request HTTP (metodo, uri, headers, body).
- `src/domain/entities/HttpRequest.cpp` -> implementacao de manipulacao/serializacao de estado de request.
- `src/domain/entities/HttpResponse.hpp` -> entidade de response HTTP (status, headers, body).
- `src/domain/entities/HttpResponse.cpp` -> serializacao de resposta para wire format HTTP.
- `src/domain/entities/common/SourceLocation.hpp` -> posicao de fonte para erros (linha/coluna/contexto).
- `src/domain/entities/common/SourceLocation.cpp` -> implementacao utilitaria de localizacao de fonte.
- `src/domain/entities/config/Token.hpp` -> token de linguagem de configuracao.
- `src/domain/entities/config/Token.cpp` -> implementacao do token e metadados.
- `src/domain/entities/server/HttpBlock.hpp` -> raiz de configuracao HTTP (global + lista de servers).
- `src/domain/entities/server/ServerBlock.hpp` -> configuracao de um server (host, port, root, locations etc).
- `src/domain/entities/server/LocationBlock.hpp` -> configuracao por location/rota.

### 5.8 `src/domain/entities/ast/base/`

- `src/domain/entities/ast/base/ASTNode.hpp` -> interface/base de no AST.
- `src/domain/entities/ast/base/ASTNode.cpp` -> comportamento comum da arvore sintatica.

### 5.9 `src/domain/entities/ast/node/`

- `src/domain/entities/ast/node/ASTRoot.hpp` -> no raiz da AST de configuracao.
- `src/domain/entities/ast/node/ASTRoot.cpp` -> implementacao do no raiz.
- `src/domain/entities/ast/node/ASTBlock.hpp` -> no de bloco (`http`, `server`, `location`).
- `src/domain/entities/ast/node/ASTBlock.cpp` -> implementacao do no de bloco.
- `src/domain/entities/ast/node/ASTDirective.hpp` -> no de diretiva (chave/argumentos).
- `src/domain/entities/ast/node/ASTDirective.cpp` -> implementacao do no de diretiva.
- `src/domain/entities/ast/node/ASTValue.hpp` -> no de valor literal/lista.
- `src/domain/entities/ast/node/ASTValue.cpp` -> implementacao do no de valor.

### 5.10 `src/domain/services/request/`

- `src/domain/services/request/HttpRequestValidator.hpp` -> contrato das regras de validacao HTTP.
- `src/domain/services/request/HttpRequestValidator.cpp` -> validacoes de metodo, versao, headers e consistencia.

### 5.11 `src/domain/services/config/`

- `src/domain/services/config/Lexer.hpp` -> contrato da analise lexica do config.
- `src/domain/services/config/Lexer.cpp` -> tokenizacao do arquivo de configuracao.
- `src/domain/services/config/Parser.hpp` -> contrato da analise sintatica do config.
- `src/domain/services/config/Parser.cpp` -> parser que monta AST do config.
- `src/domain/services/config/Validator.hpp` -> contrato de validacao semantica do config.
- `src/domain/services/config/Validator.cpp` -> aplicacao de regras semanticas sobre AST/config.
- `src/domain/services/config/ConfigBuilder.hpp` -> contrato para converter AST em blocos de dominio.
- `src/domain/services/config/ConfigBuilder.cpp` -> montagem final de `HttpBlock/ServerBlock/LocationBlock`.

### 5.12 `src/domain/services/validator/`

- `src/domain/services/validator/CardinalityRuleService.hpp` -> regra de cardinalidade (quantidade permitida de diretivas).
- `src/domain/services/validator/CardinalityRuleService.cpp` -> implementacao de cardinalidade.
- `src/domain/services/validator/ConflictRuleService.hpp` -> regra de conflito entre diretivas.
- `src/domain/services/validator/ConflictRuleService.cpp` -> implementacao de conflito.
- `src/domain/services/validator/ContextRuleService.hpp` -> regra de contexto (onde diretiva pode aparecer).
- `src/domain/services/validator/ContextRuleService.cpp` -> implementacao de contexto.
- `src/domain/services/validator/DependencyRuleService.hpp` -> regra de dependencia entre diretivas.
- `src/domain/services/validator/DependencyRuleService.cpp` -> implementacao de dependencia.
- `src/domain/services/validator/ValueRuleService.hpp` -> regra de formato/faixa de valores.
- `src/domain/services/validator/ValueRuleService.cpp` -> implementacao de validacao de valores.

### 5.13 `src/domain/services/`

- `src/domain/services/DirectoryLister.hpp` -> contrato de listagem de diretorio para autoindex.
- `src/domain/services/DirectoryLister.cpp` -> geracao de HTML/lista de arquivos para resposta GET.
- `src/domain/services/statusCodeResponse.hpp` -> tabela de mensagens padrao para status HTTP.
- `src/domain/services/statusCodeResponse.cpp` -> implementacao do mapeamento status -> texto.
- `src/domain/services/ErrPage.hpp` -> estrutura/apoio para pagina de erro custom.
- `src/domain/services/statusCodeTest/main.cpp` -> executavel de teste/manual de status code (suporte).

### 5.14 `src/domain/ports/`

- `src/domain/ports/ISemanticRule.hpp` -> contrato de regra semantica aplicada no validador.

### 5.15 `src/domain/errors/common/`

- `src/domain/errors/common/CompilerError.hpp` -> erro estruturado para pipeline de compilacao de config.
- `src/domain/errors/common/CompilerError.cpp` -> implementacao de formato/manipulacao do erro.
- `src/domain/errors/common/ErrorList.hpp` -> agregado/lista de erros.
- `src/domain/errors/common/ErrorList.cpp` -> implementacao de acumulacao e formatacao de erros.

### 5.16 `src/domain/errors/request/`

- `src/domain/errors/request/HttpRequestValidationIssue.hpp` -> tipos/categorias de falha de validacao de request.

### 5.17 `src/domain/errors/validator/`

- `src/domain/errors/validator/ValidationError.hpp` -> representacao de erro de validacao semantica.
- `src/domain/errors/validator/ValidationError.cpp` -> implementacao da estrutura de erro de validacao.

### 5.18 `src/domain/value_objects/config/`

- `src/domain/value_objects/config/TokenType.hpp` -> enum de tipos de token do parser de config.
- `src/domain/value_objects/config/ASTNodeType.hpp` -> enum de tipos de no da AST.
- `src/domain/value_objects/config/ASTValueType.hpp` -> enum de tipo de valor de no AST.
- `src/domain/value_objects/config/RuleTable.hpp` -> declaracao da tabela de regras de validacao de config.
- `src/domain/value_objects/config/RuleTable.cpp` -> inicializacao/manutencao da tabela de regras.

### 5.19 `src/domain/value_objects/common/`

- `src/domain/value_objects/common/ErrorCode.hpp` -> codigos de erro canonicos de dominio.
- `src/domain/value_objects/common/ErrorSeverity.hpp` -> nivel/severidade do erro.

### 5.20 `src/domain/value_objects/`

- `src/domain/value_objects/HttpStatusCode.hpp` -> constantes/enums de status HTTP.
- `src/domain/value_objects/MimeType.hpp` -> contrato para deducao de MIME type por extensao.
- `src/domain/value_objects/MimeType.cpp` -> mapa extensao -> MIME usado nas respostas GET.
- `src/domain/value_objects/CgiEnvironment.hpp` -> estrutura de ambiente CGI (vars de execucao).
- `src/domain/value_objects/CgiResponse.hpp` -> estrutura de resposta CGI parseada.

### 5.21 `src/domain/CGI/`

- `src/domain/CGI/CgiEnvironment.cpp` -> montagem/conversao de dados do request para env CGI.
- `src/domain/CGI/CgiResponse.cpp` -> parser da saida de script CGI para cabecalhos/body/status.

### 5.22 `src/domain/network/`

- `src/domain/network/port.hpp` -> value object de porta valida.
- `src/domain/network/port.cpp` -> validacao/normalizacao de porta.
- `src/domain/network/ipAddr.hpp` -> value object de endereco IP/host.
- `src/domain/network/ipAddr.cpp` -> validacao/normalizacao de endereco.

### 5.23 `src/domain/events/`

- `src/domain/events/epollEvents.hpp` -> tipos auxiliares de evento/poll capacity.
- `src/domain/events/epollEvents.cpp` -> implementacao utilitaria de eventos.

### 5.24 `src/infrastructure/network/`

- `src/infrastructure/network/fileDescriptor.hpp` -> encapsulamento RAII de file descriptor.
- `src/infrastructure/network/fileDescriptor.cpp` -> fechamento/controle seguro de fd.
- `src/infrastructure/network/serverSocket.hpp` -> API de socket de servidor (bind/listen/accept).
- `src/infrastructure/network/serverSocket.cpp` -> implementacao de socket listener non-blocking.
- `src/infrastructure/network/clientSocket.hpp` -> API de socket de cliente com buffers.
- `src/infrastructure/network/clientSocket.cpp` -> leitura/escrita non-blocking e gerenciamento de buffer.
- `src/infrastructure/network/testHttpResponse.hpp` -> helper de teste de resposta HTTP (suporte).
- `src/infrastructure/network/testHttpResponse.cpp` -> implementacao helper de teste (suporte).

### 5.25 `src/infrastructure/io/request/`

- `src/infrastructure/io/request/HttpRequestFramer.hpp` -> API para identificar request completa no stream.
- `src/infrastructure/io/request/HttpRequestFramer.cpp` -> framing incremental (headers/body) no buffer recebido.
- `src/infrastructure/io/request/HttpRequestParser.hpp` -> API de parse raw HTTP -> `HttpRequest`.
- `src/infrastructure/io/request/HttpRequestParser.cpp` -> parser de linha inicial, headers e corpo.

### 5.26 `src/infrastructure/io/config/`

- `src/infrastructure/io/config/FileReader.hpp` -> API para leitura de arquivo de configuracao.
- `src/infrastructure/io/config/FileReader.cpp` -> leitura de arquivo para string/buffer.
- `src/infrastructure/io/config/FileValidator.hpp` -> API de validacao de acesso/integridade do arquivo.
- `src/infrastructure/io/config/FileValidator.cpp` -> checagens de existencia/permissao/tipo.
- `src/infrastructure/io/config/SemanticAnalyzer.hpp` -> API de analise semantica baseada em regras.
- `src/infrastructure/io/config/SemanticAnalyzer.cpp` -> coordenacao das regras semanticas de config.

### 5.27 `src/infrastructure/io/`

- `src/infrastructure/io/DirectoryReader.hpp` -> API utilitaria para operacoes de diretorio/arquivo.
- `src/infrastructure/io/DirectoryReader.cpp` -> leitura/listagem/verificacao de diretorio no filesystem.

### 5.28 `src/infrastructure/CGI/`

- `src/infrastructure/CGI/CgiProcessExecutor.hpp` -> API de execucao CGI via fork/pipe/execve.
- `src/infrastructure/CGI/CgiProcessExecutor.cpp` -> implementacao de processo CGI e coleta de stdout/stderr.

### 5.29 `src/infrastructure/logging/`

- `src/infrastructure/logging/Logger.hpp` -> API concreta de logging.
- `src/infrastructure/logging/Logger.cpp` -> implementacao de log em arquivo/saida padrao.

### 5.30 `src/infrastructure/common/`

- `src/infrastructure/common/ResultBase.hpp` -> base de resultado para padrao `Result<T>`.
- `src/infrastructure/common/Result.hpp` -> template de retorno `Ok/Err` para fluxo sem excecao.

### 5.31 `src/infrastructure/common/config/`

- `src/infrastructure/common/config/ConfigResult.hpp` -> tipo de resultado da compilacao de config.
- `src/infrastructure/common/config/ConfigResult.cpp` -> implementacao de utilitarios de `ConfigResult`.
- `src/infrastructure/common/config/TokenResult.hpp` -> resultado da etapa de tokenizacao.
- `src/infrastructure/common/config/TokenResult.cpp` -> implementacao de `TokenResult`.
- `src/infrastructure/common/config/LexerResult.hpp` -> resultado da etapa de lexer.
- `src/infrastructure/common/config/LexerResult.cpp` -> implementacao de `LexerResult`.
- `src/infrastructure/common/config/ASTResult.hpp` -> resultado da etapa parser/AST.
- `src/infrastructure/common/config/ASTResult.cpp` -> implementacao de `ASTResult`.
- `src/infrastructure/common/config/ValidatorResult.hpp` -> resultado da etapa de validacao semantica.
- `src/infrastructure/common/config/ValidatorResult.cpp` -> implementacao de `ValidatorResult`.
- `src/infrastructure/common/config/RuleRegistry.hpp` -> registro/catalogo de regras semanticas.
- `src/infrastructure/common/config/RuleRegistry.cpp` -> implementacao do registro de regras.

## 6) Arquivos mais centrais para entendimento rapido

- `src/interfaces/cli/main.cpp`
- `src/application/network/server.cpp`
- `src/application/network/connectionManager.cpp`
- `src/application/network/epollManager.cpp`
- `src/application/use_cases/ParseAndValidateHttpRequestUseCase.cpp`
- `src/application/methods/HttpMethodOrchestrator.cpp`
- `src/application/CGI/CgiOrchestrator.cpp`
- `src/infrastructure/io/request/HttpRequestParser.cpp`
- `src/domain/entities/HttpRequest.cpp`
- `src/domain/entities/HttpResponse.cpp`

## 7) Resumo rapido do caminho feliz

1. `main` le config e sobe server.
2. `Server` abre listeners e espera eventos no epoll.
3. `ConnectionManager` aceita cliente e le request.
4. Parser + validator transformam bytes em `HttpRequest` valida.
5. Roteamento chama CGI ou handler de metodo HTTP.
6. Handler produz `HttpResponse`.
7. Socket escreve resposta ao cliente.

## 8) Leitura guiada (sequencial) - Parte 1

### 8.1 `src/interfaces/cli/main.cpp` (entrada)

- `main.cpp:29-32` -> handler de sinal chama `Server::requestStop()`.
- `main.cpp:37-38` -> registra `SIGINT` e `SIGTERM` para shutdown gracioso.
- `main.cpp:40-41` -> inicializa `Logger` e arquivo de log.
- `main.cpp:43-48` -> valida argumento obrigatorio (`<config_file>`).
- `main.cpp:50-56` -> executa `BuildServerConfig::execute(...)` e trata erro.
- `main.cpp:58-61` -> extrai `serverConfigs` (com fallback).
- `main.cpp:69-96` -> valida `Port`/`IpAddr`, cria `Server`, verifica `isValid()` e chama `run()`.

Papel no fluxo: bootstrap completo do processo e transferencia para o loop de eventos.

### 8.2 `src/application/network/server.hpp` (contrato)

- Estado central: `_serverSockets`, `_serverConfigs`, `_epollManager`, `_connectionManager`, `_cgiOrchestrator`, `_isValid`.
- Metodos internos: `processEvents`, `handleEventByIndex`, `findServerSocketByFd`.
- API publica: `isValid()`, `run()`, `requestStop()`, `shouldStop()`.

Papel no fluxo: orquestrador de runtime nao-bloqueante (nao concentra regra HTTP de negocio).

### 8.3 `src/application/network/server.cpp` (loop)

- `server.cpp:11-60` -> construtor monta epoll + managers + listeners por `ServerBlock`.
- `server.cpp:54-56` -> adiciona listener no epoll e associa listener -> config.
- `server.cpp:75-85` -> loop principal `while (!shouldStop())` com `waitForEvents()`.
- `server.cpp:87-92` -> processa eventos e depois despacha respostas CGI prontas.
- `server.cpp:94-120` -> dispatch por evento: listener -> accept, CGI fd, write, read.

Papel no fluxo: dispatcher de eventos de I/O; delega pipeline HTTP para `ConnectionManager`.

## 9) Leitura guiada (sequencial) - Parte 2

### 9.1 `src/application/network/epollManager.hpp/.cpp`

- Estado:
- `_epollFd` -> fd do epoll criado com `epoll_create1(0)`.
- `_maxEvents` -> capacidade (via `PollCapacity`).
- `_triggeredEvents` -> buffer de eventos retornados por `epoll_wait`.
- `_readyEventsCount` -> quantidade valida na rodada atual.

- API principal:
- `addFd(fd, event)` -> `EPOLL_CTL_ADD`.
- `modifyFd(fd, event)` -> `EPOLL_CTL_MOD`.
- `removeFd(fd)` -> `EPOLL_CTL_DEL`.
- `waitForEvents()` e overload com timeout.
- `getEventFd(index)` e `getEventFlags(index)` para leitura do lote retornado.

Pontos importantes:
- `epollManager.cpp:24-25` e `:36-37` lancam excecao em falhas de `epoll_ctl`.
- `epollManager.cpp:47-57` concentra o bloqueio/espera de readiness.
- `epollManager.cpp:59-67` protege acesso por indice em `getEventFd`.

Papel no fluxo: adapter fino de epoll; isola syscall e entrega API limpa ao `Server`/`ConnectionManager`.

### 9.2 `src/application/network/connectionManager.hpp/.cpp`

Visao geral: este e o hub da pipeline HTTP.

Estado mais relevante:
- `_clients` -> tabela indexada por fd com `ClientSocket*`.
- `_parseUseCase` -> parse + validate de request.
- `_cgiOrchestrator` -> integracao com CGI assincrono.
- `_methodOrchestrator` -> despacho GET/POST/DELETE.
- `_listenerServerConfigs` e `_clientServerConfigs` -> amarram FD a `ServerBlock`.
- `_requestReadBuffers` -> buffer acumulado de stream por cliente.

Pipeline por metodo (arquivo `connectionManager.cpp`):

1. `acceptNewClient` (`:43-73`)
- aceita em loop (drain de backlog), cria `ClientSocket`, registra `EPOLLIN`, associa config do listener ao cliente.

2. `readRawRequestOrDisconnect` (`:126-145`)
- le bytes, trata fechamento (`bytesRead == 0`), acumula em `_requestReadBuffers`, tenta extrair request completa.

3. `popCompleteRequestFromBuffer` (`:147-154`)
- delega framing para `HttpRequestFramer::popCompleteRequestFromBuffer`.

4. `parseRequestOrRespondBadRequest` (`:156-183`)
- chama `_parseUseCase.execute(rawRequest, maxBodySize)`.
- em erro: monta `HttpResponse` 4xx com detalhes e agenda envio (`queueResponse`).

5. `handleCgiOrRespondBadGateway` (`:185-204`)
- decide CGI via `CgiRouteResolver`; se start falha, responde 502.

6. `handleClientRead` (`:206-232`)
- roda o pipeline completo: read -> parse -> resolve config -> CGI ou method handler -> queue response.

7. `handleClientWrite` (`:234-252`)
- flush do buffer de escrita; ao finalizar, desconecta cliente.

8. `dispatchCgiResponses` (`:268-288`)
- coleta respostas prontas do CGI e muda fd do cliente para `EPOLLOUT`.

Papel no fluxo: composicao das etapas HTTP e roteamento de resposta no mesmo event loop.

## 10) Leitura guiada (sequencial) - Parte 3

### 10.1 `src/application/use_cases/ParseAndValidateHttpRequestUseCase.hpp/.cpp`

Objetivo: encapsular a regra de aplicacao "raw HTTP -> `HttpRequest` valida".

- Estado interno:
- `_parser` (`HttpRequestParser`) para parse sintatico.
- `_validator` (`HttpRequestValidator`) para regras HTTP.
- `_lastIssue` (`HttpRequestValidationIssue`) para status/mensagem detalhada.

- Fluxo em `execute(rawRequest, maxBodySize)`:
1. `parse` do raw string (`ParseAndValidateHttpRequestUseCase.cpp:15`).
2. validacao detalhada (`:17`) com limite de body.
3. se sem erro -> `Result<HttpRequest>` de sucesso (`:19-20`).
4. se erro -> `Result<HttpRequest>` com mensagem (`:22`).

Papel no fluxo: manter parser e validador desacoplados de `ConnectionManager`, com retorno unificado (`Result<T>`).

### 10.2 `src/infrastructure/io/request/HttpRequestFramer.hpp/.cpp`

Objetivo: decidir quando o buffer de stream contem uma request completa.

Como funciona:
- encontra fim de headers (`\r\n\r\n` ou `\n\n`) (`HttpRequestFramer.cpp:8-25`).
- tenta extrair `Content-Length` de forma case-insensitive (`:35-95`).
- calcula tamanho total necessario (`header + body`) (`:115-117`).
- se buffer ainda incompleto, retorna `false` (`:119-120`).
- se completo, recorta request em `rawRequest` e consome do buffer (`:122-124`).

Papel no fluxo: evitar parse prematuro; so libera para parser quando a request inteira chegou.

### 10.3 `src/infrastructure/io/request/HttpRequestParser.hpp/.cpp`

Objetivo: converter string HTTP bruta em objeto `HttpRequest`.

Pipeline do parser:
- separa head/body por delimitador de fim de headers (`HttpRequestParser.cpp:12-28`).
- extrai request line e faz split em metodo, URI e versao (`:30-40`, `:61-69`).
- varre linhas de headers, separa em `name:value`, aplica trim e salva (`:71-101`).

Observacao importante:
- parser e tolerant para `CRLF` e `LF`.
- validacao semantica/protocolo nao fica aqui; ela acontece no `HttpRequestValidator`.

Papel no fluxo: parse sintatico e montagem da entidade, sem regras de negocio HTTP.

## 11) Leitura guiada (sequencial) - Parte 4

### 11.1 `src/domain/services/request/HttpRequestValidator.hpp/.cpp`

Objetivo: aplicar regras de protocolo HTTP apos o parse sintatico.

Regras aplicadas em `validateDetailed(...)` (`HttpRequestValidator.cpp:29-78`):

1. metodo permitido (`GET`, `POST`, `DELETE`) (`:32-35`, `:80-83`).
2. versao permitida (`HTTP/1.0` ou `HTTP/1.1`) (`:37-41`, `:85-88`).
3. URI nao vazia (`:43-46`, `:90`).
4. header `Host` obrigatorio (`:48-51`, `:92-106`).
5. `POST` exige `Content-Length` (`:53-56`, `:114-117`).
6. `Content-Length` numerico/valido (`:58-61`, `:134-149`).
7. body nao ultrapassa limite configurado (`:63-70`, `:150-165`).
8. tamanho do body bate com `Content-Length` (`:72-75`, `:167-178`).

Retorno:
- sem erro -> `HttpRequestValidationIssue::none()`.
- erro comum -> `badRequest(...)` (400).
- body excedido/mismatch -> `contentTooLarge(...)` (413).

Suporte de status:
- `src/domain/errors/request/HttpRequestValidationIssue.hpp` define `Type`, `HttpStatusCode`, e mensagem para o `ConnectionManager` responder corretamente.

Papel no fluxo: transformar inconsistencias de request em erro HTTP sem contaminar parser com regra de negocio.

### 11.2 `src/application/methods/HttpMethodOrchestrator.hpp/.cpp`

Objetivo: despacho central de metodo HTTP para o handler correto.

Fluxo:
- `configure(serverConfig)` (`HttpMethodOrchestrator.cpp:10-15`) injeta config nos 3 handlers.
- `handle(request)` (`:30-44`) faz switch por string:
- `GET` -> `_getHandler.handle(...)`.
- `POST` -> `_postHandler.handle(...)`.
- `DELETE` -> `_deleteHandler.handle(...)`.
- outros -> 405 em `buildMethodNotAllowedResponse()` (`:17-28`).

Detalhe:
- resposta 405 inclui header `Allow: GET, POST, DELETE` (`:24`).

Papel no fluxo: ponto unico de decisao de metodo na camada de aplicacao.

### 11.3 `src/application/methods/GetRequestHandler.hpp/.cpp`

Objetivo: servir arquivo estatico ou diretorio (index/autoindex) para `GET`.

Passo a passo de `handle(...)` (`GetRequestHandler.cpp:122-152`):

1. normaliza URI removendo query/fragment (`:23-33`, `:124`).
2. bloqueia path traversal (`..`) -> 403 (`:126-127`).
3. resolve melhor `location` por maior prefix match (`:35-60`, `:129`).
4. se houver redirect configurado -> resposta de redirecionamento (`:130-131`).
5. valida se metodo `GET` e permitido naquela location (`:133-135`).
6. resolve `root`, `indexFiles`, `autoindex` e caminho final (`:137-140`).
7. se for diretorio -> `serveDirectory(...)` (`:142-143`).
8. se arquivo nao existe -> 404 (`:145-146`).
9. se sem permissao leitura -> 403 (`:148-149`).
10. caso OK -> `serveFile(...)` (`:151`).

`serveFile(...)` (`:154-173`):
- detecta MIME (`MimeType`), escolhe leitura texto/binario e retorna 200 com `Content-Type`.

`serveDirectory(...)` (`:175-206`):
- tenta index (`index.html`, etc.);
- sem index e sem autoindex -> 403;
- com autoindex -> gera HTML via `DirectoryLister`.

Papel no fluxo: caminho principal de conteudo estatico em requests GET.

### 11.4 `src/application/methods/PostRequestHandler.hpp/.cpp`

Objetivo: persistir body de request em arquivo (upload simples).

Fluxo de `handle(...)` (`PostRequestHandler.cpp:162-185`):

1. normaliza URI (`:22-32`, `:164`).
2. bloqueia `..` -> 403 (`:166-167`).
3. resolve `location` por prefixo (`:34-59`, `:169`).
4. aplica redirect se configurado (`:170-171`).
5. valida metodo permitido na location (`:173-174`).
6. revalida limite de body por config (`:176-178`).
7. resolve root/filePath/targetPath (`:180-183`).
8. grava body em disco via `writeRequestBody(...)` (`:184`).

`writeRequestBody(...)` (`:119-150`):
- valida diretorio alvo e permissao;
- abre arquivo com `O_WRONLY|O_CREAT|O_TRUNC`;
- escreve em loop ate terminar;
- retorna 201 (`buildCreatedResponse`) em sucesso.

Papel no fluxo: implementacao basica de upload/salvamento para POST sem multipart parser.

### 11.5 `src/application/methods/DeleteRequestHandler.hpp/.cpp`

Objetivo: remover recurso de arquivo com `DELETE`.

Fluxo de `handle(...)` (`DeleteRequestHandler.cpp:116-147`):

1. normaliza URI (`:22-32`, `:118`).
2. bloqueia `..` -> 403 (`:120-121`).
3. resolve `location` e possivel redirect (`:123-125`).
4. valida metodo permitido (`:127-128`).
5. resolve caminho real (`:130-131`).
6. se nao existe -> 404 (`:133-134`).
7. se for diretorio -> 403 (`:136-137`).
8. valida permissao de escrita no diretorio pai (`:139-141`).
9. remove com `std::remove` (`:143-144`).
10. sucesso -> 204 sem body (`buildNoContentResponse`, `:107-114`, `:146`).

Papel no fluxo: delete seguro de arquivo simples com checks de existencia/permissao.

### 11.6 `src/application/methods/HttpResponseBuilders.hpp` (suporte direto)

Objetivo: padronizar respostas comuns entre handlers.

Utilitarios centrais:
- `buildPlainTextRedirectResponse(...)` (`HttpResponseBuilders.hpp:61-70`).
- `buildHtmlErrorResponse(...)` (`:72-101`) com tentativa de pagina de erro custom por `location/server`.
- `resolveConfiguredErrorPagePath(...)` (`:27-59`) para montar caminho final da pagina de erro.

Papel no fluxo: reduzir duplicacao e manter consistencia de erro/redirect.

## 12) Leitura guiada (sequencial) - Parte 5 (CGI)

### 12.1 `src/application/CGI/CgiRouteResolver.hpp/.cpp`

Objetivo: decidir se a request vai para CGI e montar a config de execucao.

Regras principais:
- `isCgiRequest(uri)` (`CgiRouteResolver.cpp:3-15`) considera CGI quando URI termina com `.py` ou `.php` (ignorando query string).
- `buildConfig(...)` (`:17-34`) monta `CgiRouteConfig` com:
- `scriptPath` (root do server + URI);
- `interpreterPath` (`/usr/bin/python3` para `.py`, senao `/usr/bin/php-cgi`) (`:36-46`);
- `serverName`/`serverPort`.

Papel no fluxo: ponte entre roteamento HTTP e parametros concretos de execucao CGI.

### 12.2 `src/application/CGI/CgiHandler.hpp/.cpp`

Objetivo: encapsular uma execucao CGI para uma request.

Fluxo:
- `startCgi(request)` (`CgiHandler.cpp:7-12`)
- monta `CgiEnvironment` com dados da request + rota;
- chama gateway (`InitCgiGateway`) para iniciar processo.
- `buildResponse()` (`:16-30`)
- obtem `CgiResponse` do gateway;
- converte para `HttpResponse` (status, headers, body).

Papel no fluxo: adapter de entrada/saida entre o mundo HTTP e o executor CGI.

### 12.3 `src/application/CGI/CgiOrchestrator.hpp/.cpp`

Objetivo: gerenciar varias sessoes CGI em paralelo dentro do mesmo epoll.

Pontos centrais:
- timeout global da sessao: `CGI_TIMEOUT_SECONDS = 30` (`CgiOrchestrator.cpp:5`).
- `startCgi(...)` (`:20-49`)
- cria `CgiProcessExecutor` + `CgiHandler`;
- inicia CGI;
- registra `readFd` em `EPOLLIN` e `writeFd` em `EPOLLOUT`.
- `isCgiFd(fd)` (`:51-61`) identifica se evento pertence a pipe CGI.
- `handleEvent(fd, flags)` (`:75-97`) encaminha readiness para `onWriteReady`/`onReadReady`.
- `collectFinished()` (`:99-137`)
- verifica estado da sessao (`RUNNING`, `FINISHED`, `TIMEOUT`, `ERROR`);
- monta resposta HTTP apropriada (normal/504/502);
- remove sessao encerrada.
- `cancelForClient(clientFd)` (`:139-149`) cancela sessao quando cliente desconecta.

Papel no fluxo: scheduler de lifecycle CGI integrado ao loop principal do servidor.

### 12.4 `src/infrastructure/CGI/CgiProcessExecutor.hpp/.cpp`

Objetivo: executar script CGI em processo filho com pipes nao bloqueantes.

`start(...)` (`CgiProcessExecutor.cpp:16-100`):
- prepara estado interno e buffers;
- cria pipes stdin/stdout do filho;
- marca FDs do lado pai como `O_NONBLOCK`;
- faz `fork()`;
- no filho: `dup2` stdin/stdout, `execve(interpreter, script, envp)`;
- no pai: fecha pontas nao usadas e inicia cronometro de timeout.

I/O nao bloqueante:
- `onWriteReady()` (`:106-129`) envia body da request para stdin do CGI em partes.
- `onReadReady()` (`:131-151`) le stdout do CGI para `_outputBuffer`.

Estado/finalizacao:
- `checkState()` (`:153-187`) decide `CGI_RUNNING/FINISHED/TIMEOUT/ERROR` via tempo + `waitpid(..., WNOHANG)`.
- `getResponse()` (`:189-196`) parseia `_outputBuffer` em `CgiResponse`.
- `cleanup()` (`:198-205`) e `killChildIfAlive()` (`:216-235`) garantem liberacao de recursos/processo.

Papel no fluxo: implementacao de baixo nivel (fork/pipe/exec) com controle de timeout e limpeza robusta.

### 12.5 `src/domain/CGI/CgiEnvironment.cpp`

Objetivo: montar variaveis de ambiente CGI a partir de `HttpRequest`.

Destaques:
- popula chaves CGI padrao (`REQUEST_METHOD`, `QUERY_STRING`, `CONTENT_TYPE`, `CONTENT_LENGTH`, `SCRIPT_NAME`, `PATH_INFO`, `SERVER_*`, `GATEWAY_INTERFACE`) (`CgiEnvironment.cpp:28-38`).
- converte headers HTTP para formato `HTTP_*` (`:40-59`) exceto `content-type`/`content-length`.
- gera `envp` alocado para `execve` em `toEnvArray()` (`:62-72`) e libera em `freeEnvArray()` (`:74-81`).

Papel no fluxo: traduz contexto HTTP para contrato esperado por scripts CGI.

### 12.6 `src/domain/CGI/CgiResponse.cpp`

Objetivo: parsear saida textual do script CGI para estrutura de resposta.

Fluxo de parse:
- separa headers/body por `\r\n\r\n` ou `\n\n` (`CgiResponse.cpp:13-25`).
- parseia headers linha a linha (`:29-48`), normalizando chave para uppercase.
- interpreta header `Status` para definir status HTTP final (`:53-69`), removendo-o da lista de headers comuns.

Papel no fluxo: transformar output bruto do CGI em dados utilizaveis pelo `CgiHandler` para gerar `HttpResponse`.

## 13) Leitura guiada (sequencial) - Parte 6 (Pipeline de Config)

### 13.1 `src/application/use_cases/BuildServerConfig.hpp/.cpp`

Objetivo: orquestrar o fluxo completo de compilacao da configuracao.

Sequencia de `BuildServerConfig::execute(...)` (`BuildServerConfig.cpp:29-82`):

1. chama `CompileSourceFile::execute(...)` para validar arquivo e tokenizar (`:32-34`).
2. parseia tokens para AST com `Parser(tokens).parser()` (`:38-39`).
3. monta regras semanticas via `RuleRegistry` + serviços de regra (`:50-63`).
4. roda validacao semantica com `SemanticAnalyzer` + `Validator` (`:64-67`).
5. converte AST para `HttpBlock*` via `ConfigBuilder().build(astRoot)` (`:77`).

Papel no fluxo: gateway unico de `config.conf` para estrutura de dominio consumida no `main`.

### 13.2 `src/application/use_cases/CompileSourceFile.hpp/.cpp`

Objetivo: fazer o "front-end" do config ate gerar tokens.

Passos:

1. `loadSourceFile(...)` valida arquivo (`FileValidator`) (`CompileSourceFile.cpp:36-42`).
2. le conteudo (`FileReader::readFile`) (`:44-47`).
3. instancia `Lexer` (`:48`) e retorna `LexerResult`.
4. `execute(...)` chama `lexer->tokenize()` (`:63`) e devolve `TokenResult`.

Papel no fluxo: encapsular validacao + leitura + tokenizacao com `Result` de erro.

### 13.3 `src/infrastructure/io/config/FileValidator.hpp/.cpp`

Objetivo: barrar entradas invalidas antes do parser.

Checagens em `validateFile(...)` (`FileValidator.cpp:24-39`):

1. existe (`validateExists`, `:41-46`).
2. e arquivo regular (`validateIsRegularFile`, `:48-58`).
3. nao parece binario (`validateBinaryFile`, `:60-80`, procura `\0` em amostra).
4. tem permissao de leitura (`validateReadPermission`, `:82-89`).

Papel no fluxo: primeira barreira de qualidade do arquivo de config.

### 13.4 `src/infrastructure/io/config/FileReader.hpp/.cpp`

Objetivo: leitura de arquivo texto/binario.

- `readFile(...)` (`FileReader.cpp:18-32`) le linha a linha e recompõe com `\n`.
- `readBinaryFile(...)` (`:34-55`) le tamanho total em modo binario.

Papel no fluxo: utilitario compartilhado por config, GET e erro custom page.

### 13.5 `src/domain/services/config/Lexer.hpp/.cpp`

Objetivo: transformar texto do config em stream de `Token`.

Pontos importantes:

1. navegacao: `peek/advance/isAtEnd` (`Lexer.cpp:45-70`).
2. ignora whitespace e comentarios `#...` (`:72-85`).
3. scanners de token:
4. `scanWord` (`:103-117`), `scanNumber` (`:119-133`), `scanString` (`:150-170`), `scanPath` (`:172-181`), single-char tokens (`:183-189`).
5. `tokenize()` (`:209-246`) gera vetor de tokens + `EOF_TOKEN`; acumula erros lexicos.

Papel no fluxo: etapa lexica da DSL de configuracao.

### 13.6 `src/domain/services/config/Parser.hpp/.cpp`

Objetivo: converter tokens em AST (`ASTRoot`, `ASTBlock`, `ASTDirective`, `ASTValue`).

Fluxo principal:

1. `parseValue()` valida e converte token de valor (`Parser.cpp:73-84`).
2. `parseStatement()` decide diretiva (`;`) ou bloco (`{ ... }`) (`:87-159`).
3. `parseConfig()` percorre statements ate EOF (`:163-175`).
4. `parser()` retorna `ASTResult` com erro ou AST (`:178-188`).

Papel no fluxo: estrutura sintatica do config para validacao semantica posterior.

### 13.7 `src/infrastructure/io/config/SemanticAnalyzer.hpp/.cpp`

Objetivo: aplicar regras semanticas sobre a AST.

Como funciona:

1. recebe vetor de `ISemanticRule*` no construtor (`SemanticAnalyzer.cpp:24`).
2. `_visit(...)` aplica todas as regras no no atual (`:30-34`).
3. percorre recursivamente blocos e root (`:35-50`).
4. `analyze(...)` inicia percurso (`:55`).

Papel no fluxo: mecanismo comum de traversal e aplicacao de regras.

### 13.8 `src/domain/services/config/Validator.hpp/.cpp`

Objetivo: adapter simples para executar `SemanticAnalyzer` e devolver `ValidatorResult`.

- `validate(root)` (`Validator.cpp:22-31`) chama `analyze`, devolvendo sucesso quando nao ha erros.

Papel no fluxo: fronteira limpa de validacao semantica para o use case.

### 13.9 `src/domain/services/config/ConfigBuilder.hpp/.cpp`

Objetivo: transformar AST valida em `HttpBlock/ServerBlock/LocationBlock`.

Etapas internas:

1. helper `_directive(...)` pega 1o valor de diretiva por nome (`ConfigBuilder.cpp:42-57`).
2. helper `_directiveArgs(...)` pega lista de argumentos (`:59-78`).
3. `_buildLocation(...)` mapeia `root`, `autoindex`, `allow_methods`, `return`, `error_page` (`:82-133`).
4. `_buildServer(...)` mapeia `listen`, `server_name`, `root`, `index`, `client_max_body_size`, `error_page`, `location` (`:135-173`).
5. `_buildHttp(...)` mapeia `client_max_body_size`, `error_page`, lista de `server` (`:175-214`).
6. `build(ast)` aceita root com bloco `http` ou `server` direto e retorna `HttpBlock*` (`:218-262`).

Papel no fluxo: etapa final da compilacao de config para estruturas usadas no runtime.

## 14) Leitura guiada (sequencial) - Parte 7 (Entidades Centrais)

### 14.1 `src/domain/entities/server/HttpBlock.hpp`

Campos principais:

- `clientMaxBodySize`, `host`, `errorPages` (escopo HTTP/global).
- `server` (primeiro server de conveniencia).
- `servers` (lista completa para multi-listener).

Papel no fluxo: container raiz que sai do `BuildServerConfig` e entra no `main`.

### 14.2 `src/domain/entities/server/ServerBlock.hpp`

Campos principais:

- endereco: `port`, `host`, `serverName`.
- conteudo: `root`, `index`, `clientMaxBodySize`.
- tratamento: `errorPages`, `locations`.

Papel no fluxo: unidade de configuracao por listener/servidor.

### 14.3 `src/domain/entities/server/LocationBlock.hpp`

Campos principais:

- `path`, `root`, `allowedMethods`, `autoindex`.
- redirect: `redirectUri`, `redirectCode`.
- `errorPages` por location.

Papel no fluxo: override de comportamento por rota, usado pelos handlers GET/POST/DELETE.

### 14.4 `src/domain/entities/HttpRequest.hpp/.cpp`

Objetivo: entidade de entrada HTTP usada em todo runtime.

Pontos:

1. guarda `method`, `uri`, `version`, `headers`, `body`.
2. `setHeader(...)` normaliza nome para uppercase (`HttpRequest.cpp:24-27`, `:39-47`).
3. `getHeader(...)` consulta de forma case-insensitive via normalizacao (`:31-37`).

Papel no fluxo: contrato interno unificado para parser, validador, handlers e CGI.

### 14.5 `src/domain/entities/HttpResponse.hpp/.cpp`

Objetivo: entidade de saida HTTP serializavel para socket.

Pontos:

1. guarda `status`, `headers`, `body`.
2. `setHeader(...)` tambem normaliza header para uppercase (`HttpResponse.cpp:9-12`, `:59-65`).
3. `serialize()` monta wire format HTTP (`:30-57`):
4. linha de status `HTTP/1.1 ...` via `StatusCodeResponse` (`:35`).
5. escreve headers existentes (`:39-44`).
6. injeta `Content-Length` se ausente (`:46-51`).
7. escreve separador vazio e body (`:53-54`).

Papel no fluxo: etapa final antes de `ClientSocket::flushWriteBuffer()`.

## 15) Leitura guiada (sequencial) - Parte 8 (Infra de Rede e Resposta)

### 15.1 `src/infrastructure/network/fileDescriptor.hpp/.cpp`

Objetivo: encapsular ownership de `fd` com fechamento automatico (RAII simples).

Pontos:

1. construtor com `fd` invalido por padrao (`FileDescriptor.cpp:3`).
2. destrutor chama `invalidate()` e fecha `fd` se aberto (`:11`, `:15-22`).
3. `setNonBlocking()` aplica `fcntl(F_GETFL/F_SETFL | O_NONBLOCK)` (`:24-37`).

Papel no fluxo: evitar leak de descritor em sockets cliente/servidor.

### 15.2 `src/infrastructure/network/serverSocket.hpp/.cpp`

Objetivo: encapsular socket listener.

Fluxo principal:

1. construtor cria socket TCP e marca non-blocking (`ServerSocket.cpp:10-19`).
2. aplica `SO_REUSEADDR` (`:21-27`).
3. configura `sockaddr_in` com `Port` e `IpAddr` (`:33-53`).
4. `setBind()` e `setListen()` expõem etapas de subida (`:55-69`).
5. `setAccept()` aceita conexoes (`:62`).

Papel no fluxo: ponto de entrada de novas conexoes para o `ConnectionManager`.

### 15.3 `src/infrastructure/network/clientSocket.hpp/.cpp`

Objetivo: representar conexao de cliente com buffer de escrita.

Pontos:

1. construtor com `fd` aplica non-blocking (`ClientSocket.cpp:5-9`).
2. `receiveData()` usa `recv` (`:19-25`).
3. `sendData()` usa `send` (`:27-33`).
4. `appendToWriteBuffer()` e `flushWriteBuffer()` controlam envio incremental (`:35-51`).
5. em `flushWriteBuffer`, falha de envio em non-blocking nao derruba imediatamente a conexao (`:45-47`).

Papel no fluxo: bufferizacao de resposta e envio dirigido por `EPOLLOUT`.

### 15.4 `src/domain/services/statusCodeResponse.hpp/.cpp`

Objetivo: mapear `HttpStatusCode` para reason phrase HTTP.

Pontos:

1. construtor preenche tabela de mensagens (`statusCodeResponse.cpp:3-23`).
2. `findMsg(...)` retorna texto ou "Unknown Status" (`:27-33`).
3. `statusReturn(...)` monta "<code> <message>" (`:35-40`).

Papel no fluxo: usado por `HttpResponse::serialize()` para linha de status.

### 15.5 `src/domain/value_objects/MimeType.hpp/.cpp`

Objetivo: deduzir `Content-Type` pelo sufixo do arquivo.

Pontos:

1. extrai extensao em lowercase (`MimeType.cpp:15-25`).
2. consulta tabela estaticamente inicializada (`:27-50`, `:54`).
3. fallback para `application/octet-stream` (`:59`).

Papel no fluxo: utilizado principalmente no `GetRequestHandler::serveFile()`.

### 15.6 `src/domain/services/DirectoryLister.hpp/.cpp`

Objetivo: gerar HTML de autoindex para diretorios.

Pontos:

1. `escapeHtml(...)` sanitiza caracteres especiais (`DirectoryLister.cpp:8-25`).
2. `generateHtml(uri, entries)` monta pagina com links (`:27-51`).

Papel no fluxo: resposta de listagem quando `autoindex` esta habilitado.

## 16) Leitura guiada (sequencial) - Parte 9 (AST, Result e Logging)

### 16.1 `src/domain/entities/ast/base/ASTNode.hpp/.cpp`

Objetivo: classe base de todos os nos AST.

Pontos:

1. guarda `ASTNodeType` + `SourceLocation` (`ASTNode.hpp:24-25`).
2. expõe `getType()` e `getLocation()` (`ASTNode.cpp:27-29`).
3. exige `toString(...)` virtual puro para debug/inspecao (`ASTNode.hpp:34`).

Papel no fluxo: contrato comum da arvore usada por parser/semantic analyzer/config builder.

### 16.2 `src/domain/entities/ast/node/ASTValue.hpp/.cpp`

Objetivo: representar valor literal de diretiva (`WORD/NUMBER/STRING/PATH`).

Pontos:

1. guarda `ASTValueType` e `value` (`ASTValue.hpp:22-23`).
2. construtor marca tipo de no como `AST_NODETYPE_VALUE` (`ASTValue.cpp:23-24`).
3. usado por `ASTDirective` e parametros de `ASTBlock`.

Papel no fluxo: unidade atomica de valor da DSL de configuracao.

### 16.3 `src/domain/entities/ast/node/ASTDirective.hpp/.cpp`

Objetivo: representar uma diretiva terminada por `;`.

Pontos:

1. guarda nome da diretiva + vetor de valores (`ASTDirective.hpp:27-28`).
2. destrutor libera os `ASTValue*` (`ASTDirective.cpp:30-36`).
3. parser usa `addValue(...)` para compor diretiva (`:71`).

Papel no fluxo: unidade semantica para regras e para `ConfigBuilder` mapear campos.

### 16.4 `src/domain/entities/ast/node/ASTBlock.hpp/.cpp`

Objetivo: representar bloco (`http`, `server`, `location`) com filhos.

Pontos:

1. guarda `name`, `parameters`, `children` (`ASTBlock.hpp:25-27`).
2. destrutor libera parametros e filhos (`ASTBlock.cpp:30-40`).
3. parser usa `addParameter(...)` e `addChild(...)` durante descida recursiva.

Papel no fluxo: estrutura hierarquica do config.

### 16.5 `src/domain/entities/ast/node/ASTRoot.hpp/.cpp`

Objetivo: raiz da AST com statements de topo.

Pontos:

1. armazena vetor de statements (`ASTRoot.hpp:26`).
2. destrutor libera os nos de topo (`ASTRoot.cpp:26-30`).
3. parser adiciona cada statement com `addStatement(...)` (`ASTRoot.cpp:34`).

Papel no fluxo: ponto inicial de validacao semantica e build da config.

### 16.6 `src/infrastructure/common/Result.hpp`

Objetivo: tipo generico `Result<T>` com semantica `Ok/Err`.

Pontos:

1. construtor com valor marca `_is_ok=true`.
2. construtor com string marca `_is_ok=false`.
3. API: `isOk()`, `isErr()`, `getValue()`, `getError()`.

Papel no fluxo: retorno unificado em casos de uso HTTP (parse/validate) e outros pontos da aplicacao.

### 16.7 `src/infrastructure/logging/Logger.hpp/.cpp`

Objetivo: implementacao concreta da porta `ILogger`.

Pontos:

1. `log(msg, lvl)` aplica filtro de nivel e escreve no console com cor (`Logger.cpp:105-122`).
2. `enableFileLogging()` tenta abrir arquivo em `log/` e fallback em `./` (`:124-142`).
3. gera timestamp e nome de arquivo unico (`:76-90`).

Papel no fluxo: observabilidade de bootstrap, compilacao de config e falhas de runtime.

## 17) Leitura guiada (sequencial) - Parte 10 (Regras Semanticas)

### 17.1 `src/domain/ports/ISemanticRule.hpp`

Objetivo: contrato unico das regras semanticas.

- metodo `apply(const ASTNode&, const std::string& context, ErrorList&)`.

Papel no fluxo: permite o `SemanticAnalyzer` aplicar varias regras de forma uniforme.

### 17.2 `src/domain/value_objects/config/RuleTable.hpp/.cpp`

Objetivo: estrutura de dados para contexto/cardinalidade/conflito/dependencia.

Blocos principais:

1. contexto: `addContext`, `isAllowedInContext` (`RuleTable.cpp:26-48`).
2. cardinalidade: `setCardinality`, `checkCardinality`, `getCardinality` (`:50-85`).
3. conflito: `addConflict`, `getConflicts` (`:87-116`).
4. dependencia: `addRequirement`, `getRequirements` (`:118-146`).

Papel no fluxo: "banco de regras" consumido pelos serviços semânticos.

### 17.3 `src/infrastructure/common/config/RuleRegistry.hpp/.cpp`

Objetivo: registrar as regras default da DSL de config.

No construtor (`RuleRegistry.cpp:20-26`) chama:

1. `_registerContextRuleServices()` (`:58-89`) para permissao por contexto.
2. `_registerCardinalityRules()` (`:91-109`) para minimo/maximo de ocorrencias.
3. `_registerConflictRules()` (`:111-116`) para diretivas conflitantes.
4. `_registerDependencyRules()` (`:118`) atualmente vazio.

Papel no fluxo: fonte central de regras conectada em `BuildServerConfig`.

### 17.4 `src/domain/services/validator/ContextRuleService.hpp/.cpp`

Objetivo: validar se diretiva/bloco pode aparecer no contexto atual.

- implementacao em `apply(...)` (`ContextRuleService.cpp:23-45`) consulta `RuleTable::isAllowedInContext`.

Papel no fluxo: evita diretivas em escopo incorreto (`server`, `location`, etc.).

### 17.5 `src/domain/services/validator/CardinalityRuleService.hpp/.cpp`

Objetivo: validar quantas vezes cada diretiva aparece por bloco/contexto.

Fluxo em `apply(...)` (`CardinalityRuleService.cpp:82-125`):

1. coleta filhos do no atual (`BLOCK/ROOT`).
2. conta ocorrencias por nome.
3. pega limites min/max da `RuleTable`.
4. em violacao, gera `directiveCardinalityError`.

Papel no fluxo: garante presenca/quantidade esperada de diretivas.

### 17.6 `src/domain/services/validator/ConflictRuleService.hpp/.cpp`

Objetivo: detectar diretivas mutuamente exclusivas no mesmo contexto.

- `apply(...)` (`ConflictRuleService.cpp:77-113`) cruza nomes presentes com mapa de conflitos e emite erro.

Papel no fluxo: evita combinacoes invalidas de configuracao.

### 17.7 `src/domain/services/validator/DependencyRuleService.hpp/.cpp`

Objetivo: exigir diretivas dependentes quando outra aparece.

- `apply(...)` (`DependencyRuleService.cpp:79-109`) consulta requirements e acusa ausencias.

Papel no fluxo: valida pre-condicoes entre diretivas.

### 17.8 `src/domain/services/validator/ValueRuleService.hpp/.cpp`

Objetivo: validar formato/intervalo dos valores das diretivas.

Exemplos cobertos em `apply(...)` (`ValueRuleService.cpp:175-340`):

1. `listen` porta 1..65535.
2. `host` IPv4 valido ou `localhost`.
3. `server_name` token valido.
4. `client_max_body_size` com sufixo opcional `k/m/g`.
5. `error_page` com codigos 3xx..5xx + path.
6. `allow_methods`, `autoindex`, `return`, `cgi_path`, `cgi_ext`.
7. bloco `location` com exatamente 1 parametro iniciando por `/`.

Papel no fluxo: ultima barreira semantica antes do `ConfigBuilder`.

## 18) Leitura guiada (sequencial) - Parte 11 (Erros e Resultados)

### 18.1 `src/domain/entities/common/SourceLocation.hpp/.cpp`

Objetivo: representar posicao no arquivo (arquivo/linha/coluna/tamanho).

- `toString()` retorna `arquivo:linha:coluna` (`SourceLocation.cpp:32-38`).

Papel no fluxo: contexto de erro para lexer/parser/validator.

### 18.2 `src/domain/errors/common/CompilerError.hpp/.cpp`

Objetivo: erro estruturado com codigo, severidade, mensagem, localizacao, notas e hint.

Pontos:

1. `format()` gera mensagem completa (`CompilerError.cpp:79-98`).
2. static factories para erros de arquivo, parsing e semantica (`:101-320`).

Papel no fluxo: linguagem unica de erro para toda compilacao de config.

### 18.3 `src/domain/errors/common/ErrorList.hpp/.cpp`

Objetivo: acumulador de varios `CompilerError`.

- `addError`, `hasErrors`, `getErrors`, `formatAllErrors()` (`ErrorList.cpp:27-41`).

Papel no fluxo: retorno agregado em resultados de etapas da pipeline.

### 18.4 `src/infrastructure/common/ResultBase.hpp`

Objetivo: base simples para resultados de config (`isOk/isErr`).

Papel no fluxo: padrao comum para `LexerResult`, `TokenResult`, `ASTResult`, `ValidatorResult`, `ConfigResult`.

### 18.5 `src/infrastructure/common/config/LexerResult.hpp/.cpp`

Objetivo: retorno da etapa de carregamento/lexer.

- carrega `Lexer*` em sucesso ou `ErrorList` em erro.

Papel no fluxo: transicao segura para etapa de tokenizacao.

### 18.6 `src/infrastructure/common/config/TokenResult.hpp/.cpp`

Objetivo: retorno da etapa de tokenizacao.

- `unwrap()` transfere ownership do `std::vector<Token>*` para o chamador (`TokenResult.cpp:34-43`).

Papel no fluxo: alimenta `Parser`.

### 18.7 `src/infrastructure/common/config/ASTResult.hpp/.cpp`

Objetivo: retorno da etapa de parser AST.

- carrega `ASTNode*` ou `ErrorList`.

Papel no fluxo: entrada da validacao semantica.

### 18.8 `src/infrastructure/common/config/ValidatorResult.hpp/.cpp`

Objetivo: retorno da validacao semantica.

- sucesso sem payload; erro com `ErrorList`.

Papel no fluxo: gate final antes de `ConfigBuilder`.

### 18.9 `src/infrastructure/common/config/ConfigResult.hpp/.cpp`

Objetivo: retorno final da compilacao de config para `main`.

- carrega `HttpBlock*` em sucesso ou erro estruturado.

Papel no fluxo: contrato do `BuildServerConfig::execute(...)`.

## 19) Leitura guiada (sequencial) - Parte 12 (Rede/Evento Value Objects)

### 19.1 `src/domain/network/port.hpp/.cpp`

Objetivo: encapsular porta TCP valida.

- construtor valida faixa 1..65535 (`port.cpp:5-10`).

Papel no fluxo: proteger inicializacao do socket contra portas invalidas.

### 19.2 `src/domain/network/ipAddr.hpp/.cpp`

Objetivo: encapsular endereco IP/host para bind/startup.

- hoje a validacao e minima (`ipAddr.cpp:14-19`), recusando apenas vazio.

Papel no fluxo: value object de endereco usado por `ServerSocket`.

### 19.3 `src/domain/events/epollEvents.hpp/.cpp`

Objetivo: encapsular capacidade maxima do buffer de eventos.

- `PollCapacity(amount)` exige valor > 0 (`epollEvents.cpp:3-7`).

Papel no fluxo: parametro forte para `EpollManager` no runtime.

## 20) Leitura guiada (sequencial) - Parte 13 (Suporte Restante)

### 20.1 Interfaces e contratos

- `src/application/ports/ILogger.hpp`
Objetivo: porta de logging com `log`, `enableFileLogging`, `setFilterLevel`.
Papel no fluxo: desacoplar casos de uso da implementação concreta `Logger`.

- `src/interfaces/port/InitCgiGateway.hpp`
Objetivo: contrato de execucao CGI (start, fds, onRead/onWrite, state, response, cleanup).
Papel no fluxo: `CgiHandler` depende da interface; `CgiProcessExecutor` implementa.

- `src/domain/ports/ISemanticRule.hpp`
Objetivo: contrato das regras semanticas aplicadas sobre AST.
Papel no fluxo: plugar novas regras no `SemanticAnalyzer` sem alterar traversal.

### 20.2 Helpers operacionais

- `src/infrastructure/network/testHttpResponse.hpp`
- `src/infrastructure/network/testHttpResponse.cpp`
Objetivo: helper `bufferTestHttpResponse(...)` que serializa `HttpResponse` e coloca no write buffer do cliente.
Papel no fluxo: simplificar enfileiramento de resposta no `ConnectionManager`.

### 20.3 Enums e tipos base da DSL/config

- `src/domain/value_objects/config/TokenType.hpp`
Objetivo: enum de tokens lexicos (`WORD`, `NUMBER`, `STRING`, `PATH`, delimitadores e especiais).

- `src/domain/value_objects/config/ASTNodeType.hpp`
Objetivo: enum de tipo de no AST (`VALUE`, `DIRECTIVE`, `BLOCK`, `ROOT`).

- `src/domain/value_objects/config/ASTValueType.hpp`
Objetivo: enum do tipo de valor AST (`STRING`, `NUMBER`, `PATH`, `IDENTIFIER`, `UNDEFINED`).

- `src/domain/value_objects/common/ErrorCode.hpp`
Objetivo: taxonomia de codigos de erro da compilacao de config.

- `src/domain/value_objects/common/ErrorSeverity.hpp`
Objetivo: severidade (`WARNING`, `ERROR`, `FATAL`).

- `src/domain/value_objects/HttpStatusCode.hpp`
Objetivo: codigos HTTP usados em requests/responses e handlers.

Papel no fluxo: base semantica para parser/validator/erro/response.

### 20.4 Value objects CGI (headers)

- `src/domain/value_objects/CgiEnvironment.hpp`
Objetivo: interface do objeto que monta `envp` para `execve`.

- `src/domain/value_objects/CgiResponse.hpp`
Objetivo: interface da resposta parseada do CGI (status, headers, body).

Papel no fluxo: contrato de dominio para interoperabilidade entre camada de aplicacao e executor CGI.

## 21) Fechamento do fluxo geral (resumo final)

1. `main` carrega config (`BuildServerConfig`) e sobe `Server`.
2. `Server` executa loop de epoll e delega eventos ao `ConnectionManager`.
3. `ConnectionManager` faz framing/parsing/validacao da request.
4. roteia para CGI (`CgiOrchestrator`) ou handlers HTTP (`HttpMethodOrchestrator`).
5. handlers constroem `HttpResponse`, serializam e enviam pelo `ClientSocket`.
6. em paralelo, pipeline de config usa Lexer/Parser/Validator/ConfigBuilder para produzir `HttpBlock` seguro.










