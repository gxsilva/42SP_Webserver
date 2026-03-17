# Webserv - Status do Projeto

**Última atualização:** 2026-03-17

## 📊 Resumo Executivo

### Progresso Geral: ~45% (13/31 requisitos obrigatórios)

**✅ COMPLETO (42%):**
- Infraestrutura de rede (epoll, sockets non-blocking)
- Parsing de configuração (Lexer → Parser → AST)
- Parsing de HTTP requests (HttpRequest + HttpRequestValidator)
- HttpResponse com serialização
- CGI básico (CgiHandler, CgiEnvironment, CgiProcessExecutor)

**🔴 BLOQUEIOS CRÍTICOS:**
1. **AST não convertido em Config** → main.cpp:99 descarta astRoot
2. **CGI não integrado ao epoll** → fork() bloqueia event loop
3. **Sem Router/Dispatcher** → requests sempre retornam "Hello World"
4. **Handlers ausentes** → GET/POST/DELETE não implementados

**📋 Resumo por Categoria:**
- **Core do Servidor:** 3/8 completos
- **Métodos HTTP:** 0/3 implementados
- **Responses:** 2/3 funcionais
- **Config:** 2/11 completos (Lexer/Parser OK, resto bloqueado)
- **CGI:** 5/6 completos (falta integração com event loop)

---

## Requisitos do Subject (Cap. III)

### Core do Servidor
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 1 | Compilar com `-Wall -Wextra -Werror -std=c++98` | ✅ FEITO | Makefile configurado |
| 2 | Arquivo de configuracao como argumento | ⚠️ PARCIAL | AST criado mas main.cpp nao converte para Config, astRoot descartado linha 99 |
| 3 | Servidor nao-bloqueante | ⚠️ PARCIAL | epoll + non-blocking sockets OK, mas sem request routing |
| 4 | Usar apenas 1 poll/epoll para TODAS operacoes I/O | ⚠️ PARCIAL | EpollManager existe, mas CGI pipes nao estao integrados |
| 5 | Nunca fazer read/write sem poll() | ⚠️ PARCIAL | Sockets respeitam, CGI nao integrado ao event loop |
| 6 | fork() apenas para CGI | ✅ FEITO | fork() so esta em CgiProcessExecutor.cpp |
| 7 | Servidor nunca deve crashar | ⏸️ A TESTAR | Falta stress test, mas protecoes basicas existem |
| 8 | Ouvir em multiplas portas | ❌ NAO FEITO | Server.cpp linha 5 aceita 1 Port/IpAddr, sem suporte multi-porta |

### Metodos HTTP
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 9 | GET | ❌ NAO FEITO | HttpRequest parse/validate OK, mas sem handler de arquivos estaticos |
| 10 | POST | ❌ NAO FEITO | Parse/validate OK, sem handler de upload, apenas CGI parcial |
| 11 | DELETE | ❌ NAO FEITO | Parse/validate OK, sem handler de delecao de arquivos |

### Responses
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 12 | Classe HttpResponse | ✅ FEITO | HttpResponse.cpp/.hpp completos com serialize(), usado em connectionManager linha 67-72 |
| 13 | Paginas de erro padrao | ⚠️ PARCIAL | ErrorPageGenerator::generate() existe mas nao integrado no pipeline |
| 14 | Status codes corretos | ⚠️ PARCIAL | StatusCodeResponse + HttpStatusCode.hpp OK, mas sem routing para decidir codes |

### Arquivo de Configuracao
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 15 | Lexer (tokenizacao) | ✅ FEITO | Lexer.cpp completo e testado |
| 16 | Parser (tokens -> AST) | ✅ FEITO | Parser.cpp completo, gera ASTNode tree |
| 17 | AST -> objeto Config | ❌ **BLOCKER** | main.cpp linha 99 descarta astRoot, sem interpreter/visitor |
| 18 | Definir interface:porta | ❌ NAO FEITO | AST suporta mas Config nao existe, Server hardcoded 1 porta |
| 19 | Paginas de erro customizadas (por config) | ❌ NAO FEITO | Sem Config para mapear error_page |
| 20 | Tamanho max do corpo (client_max_body_size) | ❌ NAO FEITO | HttpRequestValidator hardcode 1MB |
| 21 | Rotas com metodos aceitos | ❌ NAO FEITO | Sem location matching, connectionManager aceita tudo |
| 22 | Redirecionamento (return) | ❌ NAO FEITO | HttpStatusCode tem 301/302 mas sem logica de redirect |
| 23 | Diretorio raiz (root) | ❌ NAO FEITO | Sem resolucao de paths estaticos |
| 24 | Listagem de diretorios (autoindex) | ❌ NAO FEITO | Sem DirectoryLister |
| 25 | Arquivo padrao (index) | ❌ NAO FEITO | Sem fallback para index.html |

### CGI
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 26 | Executar CGI baseado em extensao (.py, .php) | ✅ FEITO | CgiProcessExecutor.cpp com fork/execve/pipes linha 51 |
| 27 | CgiEnvironment (variaveis CGI) | ✅ FEITO | CgiEnvironment.cpp buildFromRequest() completo, toEnvArray() OK |
| 28 | CgiResponse (parse output) | ✅ FEITO | CgiResponse parse headers + body + status |
| 29 | CgiHandler (orquestrador) | ✅ FEITO | CgiHandler.cpp/hpp completo, buildResponse() retorna HttpResponse |
| 30 | Integrar CGI no event loop | ❌ **BLOCKER** | Pipes CGI nao adicionados ao epoll, fork bloqueia |
| 31 | Timeout de CGI | ✅ FEITO | checkState() com SIGKILL |

---

## Pipeline Atual vs Pipeline Necessario

### O que existe (funcional):
```
Config File -> Lexer -> Tokens -> Parser -> AST (astRoot descartado no main.cpp:99)

Raw HTTP -> HttpRequestParser -> HttpRequest -> HttpRequestValidator (isolado, nao integrado)

HttpRequest -> CgiEnvironment -> CgiProcessExecutor -> CgiResponse -> HttpResponse (isolado)

Sockets -> epoll -> accept -> handleClientRead -> HttpResponse hardcoded "Hello World" -> write
                                (connectionManager.cpp linha 67-72)
```

### O que falta para funcionar end-to-end:
```
✅ AST criado
❌ AST -> Config (interpreter/visitor)
     |
     v
❌ Config -> Server multi-porta (atualmente 1 porta hardcoded)
     |
     v
✅ Request chega -> epoll ready
✅ parse HTTP (HttpRequestParser OK)
❌ Router/Dispatcher (match URI -> handler)
     |
     +--> ❌ GET Handler:    FileReader + autoindex + index fallback
     +--> ❌ POST Handler:   upload + body parsing + CGI routing
     +--> ❌ DELETE Handler: file removal + permissions
     |
     v
✅ HttpResponse.serialize() -> write buffer
❌ Error pages (ErrorPageGenerator nao conectado)
```

---

## Proximos Passos Sugeridos (ordem de prioridade)

### ~~1. HttpResponse~~ ✅ JA EXISTE
- ✅ Classe completa em `src/domain/entities/HttpResponse.cpp`
- ✅ Metodos: setStatusCode, setHeader, setBody, serialize
- ✅ Usado em connectionManager mas apenas com resposta hardcoded

### 2. AST -> Config (BLOQUEIO CRITICO)
- Criar classe Config (ou ServerConfig) que represente:
  - listen (porta/interface)
  - server_name
  - error_page (map code -> path)
  - client_max_body_size
  - locations (vector de RouteConfig)
- Cada RouteConfig: root, index, autoindex, methods, return, cgi_extension, cgi_path
- Visitor/interpreter que percorre o AST e popula o Config

### 3. Request Router / Dispatcher
- Receber HttpRequest + Config
- Match da URI contra locations (prefix matching)
- Decidir handler: StaticFileHandler, CgiHandler, RedirectHandler, ErrorHandler
- Integrar em connectionManager.handleClientRead (substituir hardcoded response)

### 4. StaticFileHandler (GET)
- Resolver path: Config.root + HttpRequest.getUri()
- Se diretorio:
  - Tentar Config.index (index.html)
  - Se Config.autoindex: gerar listagem HTML
  - Senao: 403 Forbidden
- Se arquivo: FileReader + Content-Type (MIME detection)
- Se nao existe: 404

### 5. UploadHandler (POST)
- Parse multipart/form-data ou application/x-www-form-urlencoded
- Salvar arquivo no Config.upload_path
- Validar Content-Length vs Config.client_max_body_size
- Retornar 201 Created ou 413 Payload Too Large

### 6. DeleteHandler (DELETE)
- Resolver path: Config.root + URI
- Validar permissoes (nao permitir delete fora do root)
- unlink() e retornar 204 No Content ou 404

### 7. Integrar CGI no epoll (BLOQUEIO CRITICO)
- CgiProcessExecutor retornar fds dos pipes (stdin/stdout)
- ConnectionManager adicionar pipe_stdout ao epoll com EPOLLIN
- Processar output incremental sem blocking reads
- Timeout: remover do epoll + SIGKILL

### 8. Multiplas portas/server blocks
- Config retornar vector<ServerConfig>
- main.cpp criar vector<Server*>, cada um com sua porta
- Adicionar todos ServerSocket fds no mesmo EpollManager
- connectionManager identificar qual Config usar por fd

---

## Bugs/Issues Conhecidos

| # | Arquivo | Issue | Severidade |
|---|---------|-------|------------|
| 1 | `HttpRequestValidator.cpp` | Valida `HTTP/1.0` mas servidor e HTTP/1.1 | MEDIA |
| 2 | `ipAddr.cpp` | `isValidIp()` so verifica non-empty, sem validacao real | MEDIA |
| 3 | `TokenResult.cpp` | Destructor nao deleta tokens (comentado) | BAIXA (leak) |
| 4 | `main.cpp:100` | `delete astRoot` comentado, leak do AST | MEDIA (leak) |
| 5 | `main.cpp:99` | `(void)astRoot` descarta o AST sem usar | ALTA (blocker) |
| 6 | `connectionManager.cpp:67-72` | HttpResponse hardcoded "Hello, World!" | ALTA (placeholder) |
| 7 | `Server.cpp:5` | Construtor aceita 1 Port, sem suporte multi-porta | ALTA |
| 8 | `CgiProcessExecutor` | fork/exec nao integrado ao epoll, bloqueia event loop | CRITICA |
| 9 | `interfaces/old_main.cpp` | Codigo morto | BAIXA |
| 10 | `ErrorPageGenerator` | Existe mas nunca usado no pipeline real | MEDIA |

---

## Arquitetura (DDD + Hexagonal) - Resumo

```
interfaces/          -> Entry points (main, ports/interfaces)
application/         -> Use cases, orchestration (CgiHandler, Server, ConnectionManager)
domain/              -> Entidades, value objects, services (HttpRequest, CgiEnvironment, Lexer, Parser)
infrastructure/      -> Implementacoes concretas (sockets, I/O, CgiProcessExecutor, Logger)
```

Fluxo de dependencia: interfaces -> application -> domain <- infrastructure

---

## 🧪 O Que Testar Agora (antes de continuar implementação)

### Testes Unitários Existentes
```bash
# Verificar se existem testes compilados
make test 2>/dev/null || echo "Sem target 'test' no Makefile"
find test/ -name "*.cpp" -o -name "*test*" | head -10
```

### Testar Componentes Isolados
1. **Lexer/Parser:**
   - `./webserv config.conf` deve gerar AST sem crashar
   - Conferir se tokens estão corretos

2. **HttpRequestParser:**
   - Testar parsing de GET/POST/DELETE válidos e inválidos
   - Verificar validação de headers, URI, versão HTTP

3. **HttpResponse:**
   - Testar serialize() gera HTTP/1.1 válido
   - Verificar auto-inserção de Content-Length

4. **CGI isolado:**
   - Testar CgiEnvironment.toEnvArray()
   - Testar CgiProcessExecutor com script Python simples
   - Verificar timeout mata processo corretamente

### Stress Test (antes de entregar)
```bash
# Teste de carga básico
ab -n 1000 -c 10 http://localhost:8080/

# Teste de requests grandes
dd if=/dev/urandom bs=1M count=10 | curl -X POST --data-binary @- http://localhost:8080/upload

# Teste de slow loris (keep-alive abuse)
slowhttptest -c 1000 -H -g -o slow.html -i 10 -r 200 -t GET -u http://localhost:8080/
```

---

## �� Referências

- **Subject:** `subject.txt`
- **NGINX Behavior:** Usar como referência para edge cases HTTP
- **CGI/1.1 Spec:** RFC 3875
- **HTTP/1.1 Spec:** RFC 2616 (legacy) ou RFC 7230-7235 (atualizado)
- **DDD Principles:** Seguir separação domain/application/infrastructure
- **Object Calisthenics:** Aplicar quando melhora legibilidade

---

## 🚀 Próxima Sessão de Trabalho

**Prioridade 1:** Implementar AST → Config interpreter
**Prioridade 2:** Criar Router/Dispatcher básico
**Prioridade 3:** Implementar StaticFileHandler (GET)
