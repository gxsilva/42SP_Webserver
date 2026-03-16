# Webserv - Project Status

Data da revisão: 2026-03-16
Escopo: varredura completa de `src/` — rede, HTTP, CGI, config, conformidade com subject.

## Resumo executivo

- **Config pipeline COMPLETO**: Lexer + Parser + AST + SemanticAnalyzer + ConfigBuilder produz HttpBlock/ServerBlock/LocationBlock.
- **CGI INTEGRADO ao event loop**: CgiOrchestrator gerencia múltiplas sessões CGI concorrentes, pipes registrados no epoll, timeout de 30s, cleanup com SIGKILL.
- **HTTP parse/validate CONECTADO ao runtime**: ConnectionManager usa ParseAndValidateHttpRequestUseCase para processar requests reais.
- **HttpResponse funcional**: serialização HTTP/1.1 com headers e body.
- **Gap principal**: servir arquivos estáticos (GET/POST/DELETE handlers), config -> Server bootstrap, multi-porta.
- Regra de `fork()` apenas para CGI: aderente.
- Sem uso de `errno` pós-I/O: aderente.

## Estado por área

### 1) Infra de rede e event loop
- **Status:** COMPLETO (base)
- Implementado:
  - `EpollManager` com `epoll_create1` + `epoll_wait` (timeout 1s)
  - `Server.run()` com loop de eventos e dispatch para handlers
  - `ConnectionManager` com accept/read/write por eventos
  - `ServerSocket` non-blocking com SO_REUSEADDR
  - `ClientSocket` com write buffer e flush
  - `FileDescriptor` RAII wrapper
  - CGI pipe FDs registrados no mesmo epoll
- Pendente:
  - **Config -> Server bootstrap** (main.cpp só processa config, não inicia Server)
  - **Multi-porta/server blocks** (HttpBlock suporta apenas 1 ServerBlock)
  - Valores hardcoded no ConnectionManager (porta 8080, interpreter paths)

### 2) HTTP request/response
- **Status:** INTEGRADO ao runtime
- Implementado:
  - `HttpRequestParser` (request line, headers, body, CRLF/LF)
  - `HttpRequest` (método, URI, headers normalizados, body)
  - `HttpRequestValidator` (métodos GET/POST/DELETE, URI, Host, Content-Length)
  - `ParseAndValidateHttpRequestUseCase` (orquestra parse + validação)
  - `HttpResponse` com `serialize()` (status line, headers, Content-Length auto, body)
  - `StatusCodeResponse` (códigos 200-505 mapeados)
  - Pipeline conectado ao ConnectionManager.handleClientRead()
- Pendente:
  - **Handlers reais de GET/POST/DELETE** (servir arquivos, upload, delete)
  - **Autoindex** (listagem de diretório)
  - **Redirect** (301/302)
  - **Páginas de erro customizadas** (structs existem — ErrPage, errorPages em ServerBlock/LocationBlock)
  - **HTTP/1.1**: validator rejeita HTTP/1.1 (aceita apenas HTTP/1.0)
  - Resposta para requests estáticos é stub: `"Static file logic goes here!"`

### 3) Configuração (arquivo .conf)
- **Status:** COMPLETO (pipeline inteiro)
- Implementado:
  - `FileValidator` (existência, permissões, binário, leitura)
  - `FileReader` (conteúdo inteiro em string)
  - `Lexer` (WORD, NUMBER, STRING, PATH, LBRACE, RBRACE, SEMICOLON, etc.)
  - `Parser` (recursive descent -> AST com ASTRoot/ASTBlock/ASTDirective/ASTValue)
  - `SemanticAnalyzer` + regras (Context, Cardinality, Conflict, Dependency, Value)
  - `RuleRegistry` (todas as diretivas: server, listen, host, root, index, error_page, allow_methods, autoindex, return, cgi_path, cgi_ext, upload_path, client_max_body_size, server_name, location)
  - `ConfigBuilder` (AST -> HttpBlock com ServerBlock e LocationBlocks)
  - `CompileSourceFile` use case (orquestra pipeline)
  - `main.cpp` processa config completa e gera HttpBlock
- Pendente:
  - **HttpBlock passar para o Server** (wiring no main.cpp)
  - **HttpBlock suportar múltiplos ServerBlocks** (atualmente tem apenas 1)

### 4) CGI
- **Status:** COMPLETO E INTEGRADO
- Implementado:
  - `CgiEnvironment` (variáveis CGI: REQUEST_METHOD, QUERY_STRING, CONTENT_*, SCRIPT_NAME, PATH_INFO, SERVER_*, GATEWAY_INTERFACE, HTTP_*)
  - `CgiResponse` (parse headers/body/status, CRLF e LF)
  - `CgiProcessExecutor` (pipe, fork/execve, non-blocking I/O, timeout, SIGKILL cleanup) — implementa `InitCgiGateway`
  - `CgiHandler` (request -> env -> executor -> response) com `CgiRouteConfig`
  - `CgiOrchestrator` (múltiplas sessões concorrentes, FDs no epoll, collectFinished, cancelForClient)
  - Detecção por extensão (.py/.php) em `ConnectionManager.isCgiRequest()`
  - Dispatch de respostas CGI em `ConnectionManager.dispatchCgiResponses()`
- Pendente:
  - Roteamento CGI baseado em config (cgi_path/cgi_ext do LocationBlock) em vez de hardcoded
  - Testar com carga concorrente real

### 5) Testes
- **Status:** BOM (unitários)
- Implementados:
  - `HttpRequestTest` (12 testes) — parser e entidade
  - `HttpRequestValidationTest` (11 testes) — validação de requests
  - `ParseAndValidateHttpRequestUseCaseTest` (5 testes) — integração parse+validate
  - `CgiTest` (14 testes) — CgiEnvironment + CgiResponse
  - `statusCodeTest` — testes standalone do StatusCodeResponse
- Pendente:
  - Testes de integração end-to-end (request real -> response real)
  - Testes de config pipeline (Lexer -> Parser -> ConfigBuilder)
  - Testes do Server/ConnectionManager com mock

## Conformidade com regras do subject

| Regra | Status | Notas |
|-------|--------|-------|
| Non-blocking, 1 poll/epoll para todo I/O | **OK** | EpollManager único, rede + CGI pipes |
| Sem read/write fora do poll | **OK** | Toda I/O via epoll readiness |
| Sem errno pós-I/O | **OK** | Não encontrado no src/ |
| fork() apenas para CGI | **OK** | Único fork em CgiProcessExecutor |
| Métodos GET/POST/DELETE | **PARCIAL** | Parser aceita, handlers não implementados |
| Múltiplas portas | **PENDENTE** | Server usa porta única |
| Páginas de erro padrão | **PENDENTE** | Structs existem, não conectadas |
| Config file como argumento | **PARCIAL** | Compila config, não usa no Server |
| Listagem de diretórios (autoindex) | **PENDENTE** | Flag no LocationBlock, sem handler |
| Redirecionamento HTTP | **PENDENTE** | Campo no LocationBlock, sem handler |
| Upload de arquivos | **PENDENTE** | Campo no LocationBlock, sem handler |
| CGI baseado em extensão | **PARCIAL** | Funciona hardcoded, não usa config |
| client_max_body_size | **PENDENTE** | Está na config, não aplicado no runtime |

## Problemas identificados

1. **HttpRequestValidator rejeita HTTP/1.1** — `validateVersion()` aceita apenas HTTP/1.0. O subject exige HTTP/1.1.
2. **HttpBlock suporta apenas 1 ServerBlock** — precisa ser `std::vector<ServerBlock>`.
3. **IpAddr validation mínima** — só checa non-empty, sem validação de formato.
4. **ASTValueType typo** — `AST_VALUE_indentationIFIER` deveria ser `AST_VALUE_IDENTIFIER`.
5. **main.cpp não inicia o Server** — processa config e termina.
6. **Valores hardcoded no ConnectionManager** — porta 8080, paths de interpreters.

## Próximos passos das conexões (foco atual)

### Fase 1: Wiring config -> Server (prioridade máxima)
1. Modificar `main.cpp` para: compilar config **E** iniciar `Server` com os dados do `HttpBlock`
2. `Server` receber `HttpBlock` (ou `ServerBlock`) no construtor
3. `ConnectionManager` ler porta, host, root, locations da config em vez de hardcoded
4. Suportar `std::vector<ServerBlock>` no `HttpBlock` para múltiplos servidores

### Fase 2: Request routing baseado em config
5. Implementar **Router** que recebe o request e o `ServerBlock`, encontra o `LocationBlock` correto
6. Router decide: servir arquivo estático, redirect, CGI, autoindex, ou erro
7. CGI routing usar `cgi_path`/`cgi_ext` do `LocationBlock` em vez de extensão hardcoded

### Fase 3: Handlers de arquivo estático
8. **GET handler**: resolver path (root + URI), servir arquivo, Content-Type por extensão
9. **DELETE handler**: remover arquivo, responder 200/204 ou 404
10. **POST handler**: upload de arquivo para `upload_path`
11. **Autoindex**: gerar HTML com listagem do diretório quando `autoindex on`
12. **Redirect**: responder 301/302 com Location header quando `return` configurado
13. **Error pages**: servir páginas customizadas de `error_page` ou default

### Fase 4: Correções de conformidade
14. Aceitar HTTP/1.1 no validator (manter compatibilidade com 1.0)
15. Aplicar `client_max_body_size` no parse de requests
16. Múltiplas portas — 1 ServerSocket por porta, todos no mesmo epoll


Próximos passos das conexões (4 fases no PROJECT_STATUS.md)
Fase	Foco
1	Wiring config -> Server — main.cpp iniciar Server com HttpBlock, ConnectionManager ler config
2	Router baseado em config — encontrar LocationBlock correto, decidir handler
3	Handlers reais — GET (servir arquivo), POST (upload), DELETE, autoindex, redirect, error pages
4	Correções de conformidade — HTTP/1.1, client_max_body_size, múltiplas portas