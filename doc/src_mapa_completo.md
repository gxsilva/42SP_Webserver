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

