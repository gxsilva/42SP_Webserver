# Webserv - Status do Projeto

## Requisitos do Subject (Cap. III)

### Core do Servidor
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 1 | Compilar com `-Wall -Wextra -Werror -std=c++98` | FEITO | Makefile configurado |
| 2 | Arquivo de configuracao como argumento | PARCIAL | Config e lida e parseada ate AST, mas nao vira objeto Config usavel |
| 3 | Servidor nao-bloqueante | PARCIAL | epoll + non-blocking sockets existem, mas pipeline incompleto |
| 4 | Usar apenas 1 poll/epoll para TODAS operacoes I/O | PARCIAL | EpollManager existe, mas CGI pipes nao estao integrados ao epoll |
| 5 | Nunca fazer read/write sem poll() | PARCIAL | Sockets respeitam, CGI ainda nao integrado |
| 6 | fork() apenas para CGI | FEITO | fork() so esta em CgiProcessExecutor |
| 7 | Servidor nunca deve crashar | A TESTAR | Falta stress test |
| 8 | Ouvir em multiplas portas | NAO FEITO | Server so aceita 1 porta |

### Metodos HTTP
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 9 | GET | NAO FEITO | HttpRequest parse/validate OK, mas nao ha handler que serve arquivos |
| 10 | POST | NAO FEITO | Parse/validate OK, mas nao ha handler de upload/processamento |
| 11 | DELETE | NAO FEITO | Parse/validate OK, mas nao ha handler de delecao |

### Responses
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 12 | Classe HttpResponse | NAO FEITO | **Blocker** - CgiHandler referencia mas nao existe |
| 13 | Paginas de erro padrao | PARCIAL | ErrorPageGenerator existe mas nao esta integrado |
| 14 | Status codes corretos | PARCIAL | StatusCodeResponse mapeia codigos, mas nao ha quem use em producao |

### Arquivo de Configuracao
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 15 | Lexer (tokenizacao) | FEITO | Completo e testado |
| 16 | Parser (tokens -> AST) | FEITO | Completo e testado |
| 17 | AST -> objeto Config | NAO FEITO | Proximo passo critico - AST existe mas nao vira Config |
| 18 | Definir interface:porta | NAO FEITO | AST suporta mas Config nao existe |
| 19 | Paginas de erro customizadas (por config) | NAO FEITO | |
| 20 | Tamanho max do corpo (client_max_body_size) | NAO FEITO | Validator tem hardcode 1MB mas nao vem do config |
| 21 | Rotas com metodos aceitos | NAO FEITO | |
| 22 | Redirecionamento (return) | NAO FEITO | StatusCode 301/302 definidos mas sem logica |
| 23 | Diretorio raiz (root) | NAO FEITO | |
| 24 | Listagem de diretorios (autoindex) | NAO FEITO | |
| 25 | Arquivo padrao (index) | NAO FEITO | |

### CGI
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 26 | Executar CGI baseado em extensao (.py, .php) | FEITO | CgiProcessExecutor completo com fork/exec/pipes |
| 27 | CgiEnvironment (variaveis CGI) | FEITO | REQUEST_METHOD, QUERY_STRING, CONTENT_TYPE, etc. |
| 28 | CgiResponse (parse output) | FEITO | Headers + body + Status |
| 29 | CgiHandler (orquestrador) | PARCIAL | Logica OK mas depende de HttpResponse que nao existe |
| 30 | Integrar CGI no event loop | NAO FEITO | Pipes do CGI nao estao no epoll |
| 31 | Timeout de CGI | FEITO | checkState() com SIGKILL |

---

## Pipeline Atual vs Pipeline Necessario

### O que existe (funcional):
```
Config File -> Lexer -> Tokens -> Parser -> AST  (para aqui)

Raw HTTP -> HttpRequestParser -> HttpRequest -> HttpRequestValidator  (funciona isolado)

HttpRequest -> CgiEnvironment -> CgiProcessExecutor -> CgiResponse  (funciona isolado)

Sockets -> epoll -> accept/read/write -> "Hello World"  (resposta hardcoded)
```

### O que falta para funcionar end-to-end:
```
AST -> Config (falta criar)
     |
     v
Server(Config) -> escutar multiplas portas (falta)
     |
     v
Request chega -> parse HTTP -> route matching (falta)
     |
     +--> GET:    ler arquivo / autoindex / index.html (falta)
     +--> POST:   upload / CGI (parcial - falta integracao)
     +--> DELETE:  deletar arquivo (falta)
     |
     v
HttpResponse (falta criar) -> enviar ao client
```

---

## Proximos Passos Sugeridos (ordem de prioridade)

### 1. HttpResponse (blocker)
- Criar classe com: status code, headers map, body
- Metodos: setStatusCode, setHeader, setBody, serialize (gerar string HTTP/1.1)
- Desbloqueia: CgiHandler, GET handler, error pages, tudo

### 2. AST -> Config
- Criar classe Config (ou ServerConfig) que represente:
  - listen (porta/interface)
  - server_name
  - error_page (map code -> path)
  - client_max_body_size
  - locations (vector de RouteConfig)
- Cada RouteConfig: root, index, autoindex, methods, return, cgi_extension, cgi_path
- Visitor/interpreter que percorre o AST e popula o Config

### 3. Request Router
- Recebe HttpRequest + Config
- Match da URI contra locations
- Decide: arquivo estatico, CGI, redirect, erro

### 4. GET Handler
- Resolver path: root + URI
- Se diretorio: tentar index file, senao autoindex ou 403
- Se arquivo: ler e retornar com Content-Type
- Se nao existe: 404

### 5. POST Handler
- Se rota CGI: delegar ao CgiHandler
- Se upload: salvar arquivo no path
- Se nenhum: 405

### 6. DELETE Handler
- Resolver path: root + URI
- Se arquivo existe: deletar, retornar 204
- Se nao: 404

### 7. Integrar CGI no epoll
- Adicionar pipe fds do CGI ao epoll
- Processar onReadReady/onWriteReady via eventos

### 8. Multiplas portas/server blocks
- Iterar configs, criar ServerSocket por porta
- Todas no mesmo epoll

---

## Bugs/Issues Conhecidos

| # | Arquivo | Issue | Severidade |
|---|---------|-------|------------|
| 1 | `HttpRequestValidator.cpp` | Valida `HTTP/1.0` mas servidor e HTTP/1.1 | MEDIA |
| 2 | `ipAddr.cpp` | `isValidIp()` so verifica non-empty | BAIXA |
| 3 | `TokenResult.cpp` | Destructor nao deleta tokens (comentado) | BAIXA (leak) |
| 4 | `main.cpp:100` | `delete astRoot` comentado | BAIXA (leak) |
| 5 | `interfaces/old_main.cpp` | Codigo morto | BAIXA |
| 6 | `testHttpResponse.cpp` | Resposta hardcoded "Hello World" | ESPERADO (placeholder) |

---

## Arquitetura (DDD + Hexagonal) - Resumo

```
interfaces/          -> Entry points (main, ports/interfaces)
application/         -> Use cases, orchestration (CgiHandler, Server, ConnectionManager)
domain/              -> Entidades, value objects, services (HttpRequest, CgiEnvironment, Lexer, Parser)
infrastructure/      -> Implementacoes concretas (sockets, I/O, CgiProcessExecutor, Logger)
```

Fluxo de dependencia: interfaces -> application -> domain <- infrastructure
