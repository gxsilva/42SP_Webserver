# Webserv - Project Status

Data da revisão: 2026-03-13  
Escopo: varredura de `src/` com foco em CGI e conformidade das regras do subject.

## Resumo executivo

- O projeto já possui base sólida de parser de config (Lexer + Parser + AST), parser/validador HTTP, camada de sockets com `epoll` e módulos de CGI (`CgiEnvironment`, `CgiResponse`, `CgiProcessExecutor`, `CgiHandler`).
- `HttpResponse` existe e está funcional (serialização HTTP/1.1, headers e body).
- O maior gap atual é **integração end-to-end**: o runtime de rede ainda responde com payload fixo e não roteia requisições para handlers HTTP/CGI.
- Regra de `fork()` apenas para CGI está aderente (único `fork()` em `CgiProcessExecutor`).
- Uso direto de `errno` no `src/` não foi encontrado na varredura atual.

## Estado por área

### 1) Infra de rede e event loop
- **Status:** PARCIAL
- Implementado:
  - `EpollManager` com `epoll_create1` + `epoll_wait`
  - `Server` e `ConnectionManager` com accept/read/write por eventos
  - Sockets non-blocking via `ClientSocket`
- Pendente:
  - Fluxo de request real (parse + roteamento + handlers)
  - Multi-porta/server blocks
  - Integração de FDs de CGI no loop principal

### 2) HTTP request/response
- **Status:** PARCIAL (base pronta)
- Implementado:
  - `HttpRequest`, parser e validação (`ParseAndValidateHttpRequestUseCase`)
  - `HttpResponse` com serialização
- Pendente:
  - Handlers de `GET/POST/DELETE` integrados ao servidor
  - Respostas de erro/arquivos/autoindex conectadas ao runtime

### 3) Configuração (arquivo .conf)
- **Status:** PARCIAL
- Implementado:
  - Lexer, Parser e AST
- Pendente:
  - Transformação AST -> objeto de configuração executável
  - Aplicação da config no bootstrap do servidor

### 4) CGI
- **Status:** PARCIAL AVANÇADO
- Implementado:
  - `CgiEnvironment` (variáveis CGI)
  - `CgiResponse` (parse de headers/body/status)
  - `CgiProcessExecutor` (pipe, fork/execve, timeout, cleanup)
  - `CgiHandler` (orquestra request -> gateway -> response)
- Pendente:
  - Uso real de `CgiHandler` no fluxo de rede (atualmente não está conectado)
  - Registro dos pipes de CGI no event loop principal
  - Estratégia de lifecycle para múltiplos CGI concorrentes

## Conformidade com regras críticas do subject

### Não-bloqueante e I/O orientado a evento
- **Parcialmente aderente.**
- Rede principal usa `epoll`, mas o CGI ainda não está integrado ao mesmo loop de eventos do servidor.

### `fork()` apenas para CGI
- **Aderente.**
- Apenas em `src/infrastructure/CGI/CgiProcessExecutor.cpp`.

### Uso de `errno` para controle de I/O
- **Aderente no estado atual do `src/`.**
- Não foram encontrados usos de `errno` na varredura textual atual.

## Testes

- Existem alvos de teste no `Makefile` para:
  - `HttpRequest`
  - `HttpRequestValidation`
  - `ParseAndValidateHttpRequestUseCase`
  - `CgiTest`
- `CgiTest` cobre `CgiEnvironment` e `CgiResponse` (nível de unidade).

## Próximos passos recomendados (prioridade)

1. Integrar pipeline de request no `ConnectionManager` (parse + validação + roteamento).
2. Conectar `CgiHandler` ao runtime e registrar FDs de CGI no `epoll`.
3. Implementar camada AST -> Config e inicialização de servidor por configuração.
4. Substituir resposta hardcoded por `HttpResponse` produzido por handlers reais.
5. Consolidar testes de integração (request real -> resposta real, com e sem CGI).
