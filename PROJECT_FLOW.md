# Fluxo atual do projeto (estado até agora)

Última atualização: 2026-03-16
Este documento descreve o fluxo **como o código está hoje** e o fluxo-alvo para chegar ao comportamento esperado do webserver.

## 1) Fluxo de configuração

### O que já existe (COMPLETO)

```text
Arquivo .conf
  -> FileValidator (existência, permissões, binário)
  -> FileReader
  -> Lexer (tokenização completa)
  -> Parser (recursive descent -> AST)
  -> SemanticAnalyzer (regras: contexto, cardinalidade, conflito, dependência, valor)
  -> ConfigBuilder (AST -> HttpBlock/ServerBlock/LocationBlock)
```

### O que ainda falta

```text
ConfigBuilder produz HttpBlock, mas:
  -> main.cpp ainda NÃO inicializa o Server com base na config
  -> HttpBlock suporta apenas 1 ServerBlock (sem multi-server) //suave
  -> Valores hardcoded no ConnectionManager (porta 8080, paths de interpreters)
  -> Config precisa alimentar o Server no bootstrap
```

## 2) Fluxo de rede (runtime atual)

```text
ServerSocket (listen, non-blocking, SO_REUSEADDR)
  -> EpollManager (epoll_create1 + epoll_wait, timeout 1s)
  -> Server.run() (loop principal de eventos)
      -> processEvents() -> handleEventByIndex()
          -> novo cliente: ConnectionManager.acceptNewClient()
          -> EPOLLIN: ConnectionManager.handleClientRead()
              -> ParseAndValidateHttpRequestUseCase
              -> isCgiRequest(.py/.php)? -> CgiOrchestrator.startCgi()
              -> senão -> resposta stub "Static file logic goes here!"
          -> EPOLLOUT: ConnectionManager.handleClientWrite()
              -> ClientSocket.flushWriteBuffer()
      -> CgiOrchestrator.collectFinished() -> dispatchCgiResponses()
```

O pipeline HTTP está conectado: request é parseado e validado. CGI funciona end-to-end via CgiOrchestrator. O gap principal é **servir arquivos estáticos** (GET/POST/DELETE handlers reais).

## 3) Fluxo HTTP (integrado ao runtime)

```text
Raw HTTP request (dados do ClientSocket)
  -> HttpRequestParser.parse()
  -> HttpRequest (método, URI, headers, body)
  -> HttpRequestValidator.validate()
      - métodos: GET, POST, DELETE
      - versão: HTTP/1.0 apenas (rejeita HTTP/1.1 — precisa corrigir)
      - valida URI, Host header, Content-Length
  -> Se válido: roteamento (CGI ou estático)
  -> HttpResponse.serialize() -> ClientSocket write buffer
```

## 4) Fluxo CGI (INTEGRADO ao event loop)

### Componentes
- `CgiEnvironment`: monta variáveis CGI a partir do `HttpRequest`
- `CgiProcessExecutor`: pipe + fork/execve, non-blocking I/O, timeout, cleanup (SIGKILL)
- `CgiResponse`: parseia output CGI (headers/body/status, CRLF e LF)
- `CgiHandler`: orquestra request -> env -> executor -> response
- `CgiOrchestrator`: gerencia múltiplas sessões CGI concorrentes no epoll

### Fluxo end-to-end de CGI

```text
HttpRequest com URI .py/.php
  -> ConnectionManager.isCgiRequest()
  -> CgiOrchestrator.startCgi(clientFd, request, config)
      -> CgiHandler.startCgi()
          -> CgiEnvironment.buildFromRequest()
          -> CgiProcessExecutor.start() (fork + execve)
      -> registra pipe FDs no EpollManager
  -> Server.run() processa eventos dos pipes CGI
      -> CgiOrchestrator.handleEvent()
          -> onWriteReady() (envia body ao CGI)
          -> onReadReady() (lê output do CGI)
      -> CgiOrchestrator.collectFinished() (timeout 30s, estados: FINISHED/TIMEOUT/ERROR)
  -> ConnectionManager.dispatchCgiResponses()
      -> CgiHandler.buildResponse()
      -> CgiResponse.parse(output)
      -> HttpResponse -> ClientSocket write buffer
```

## 5) Fluxo alvo (próxima versão funcional)

```text
Cliente conecta
  -> epoll (readiness)
  -> ler request
  -> parse + validate (FEITO)
  -> router baseado em config (location/método)
      -> GET handler: servir arquivo, autoindex, redirect
      -> POST handler: upload, CGI (FEITO para CGI)
      -> DELETE handler: remover arquivo
      -> Error handler: páginas de erro customizadas
  -> HttpResponse real
  -> serializar e enviar (FEITO)
  -> fechar ou manter conexão conforme headers
```

## 6) Situação de conformidade (snapshot)

| Regra | Status |
|-------|--------|
| `fork()` apenas para CGI | OK — único fork em CgiProcessExecutor |
| Non-blocking com `epoll` | OK — rede + CGI pipes no mesmo loop |
| I/O apenas via poll/epoll | OK — toda leitura/escrita passa pelo epoll |
| Sem uso de `errno` pós-I/O | OK — não encontrado no src/ |
| CGI integrado ao event loop | OK — CgiOrchestrator registra pipes no epoll |
| Métodos GET/POST/DELETE | PARCIAL — parser aceita, handlers não implementados |
| Múltiplas portas | PENDENTE — Server usa porta única hardcoded |
| Páginas de erro padrão | PENDENTE — structs existem mas não conectadas |
| Arquivo de configuração usado no bootstrap | PENDENTE — main.cpp não inicia Server |
| HTTP/1.1 suportado | PENDENTE — validator rejeita HTTP/1.1 |

## 7) Leitura rápida de maturidade

- **Config (Lexer/Parser/AST/Validator/Builder)**: COMPLETO
- **Rede + Event Loop**: COMPLETO (epoll + CGI integrado)
- **HTTP parse/validate/response**: COMPLETO (base)
- **CGI end-to-end**: COMPLETO (fork, pipes, timeout, cleanup)
- **Handlers de arquivo estático**: STUB — resposta hardcoded
- **Config -> Server bootstrap**: PENDENTE — gap principal
- **Multi-server/porta**: PENDENTE
- **Integração final end-to-end**: foco das próximas entregas
