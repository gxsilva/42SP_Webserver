# Webserv - Status do Projeto

**Última atualização:** 2026-03-19

## 📊 Resumo Executivo

### Progresso Geral: ~71% (22/31 requisitos obrigatórios)

**✅ COMPLETO (~71%):**
- Infraestrutura de rede (epoll, sockets non-blocking)
- Parsing de configuração (Lexer → Parser → AST)
- Build de Config em runtime (AST -> HttpBlock/ServerBlock)
- Parsing de HTTP requests (HttpRequest + HttpRequestValidator)
- HttpResponse com serialização
- GET estático (arquivo, index, autoindex, 403/404)
- POST básico de upload (criação de arquivo, 201, allow_methods)
- DELETE básico (remoção de arquivo, 204/403/404, allow_methods)
- CGI básico (CgiHandler, CgiEnvironment, CgiProcessExecutor)
- parser incremental por conexão (headers/body fragmentados)
- política de loop com `epoll_wait` bloqueante + shutdown via SIGINT/SIGTERM
- extração de roteamento CGI para `CgiRouteResolver` (sem mudança funcional)

**🔴 BLOQUEIOS CRÍTICOS:**
1. **POST ainda simplificado** → sem multipart/form-data
2. **`error_page` ainda pendente no pipeline** → falta suporte completo de páginas de erro por config
3. **CGI ainda não usa `cgi_path`/`cgi_ext` da config** → seleção por extensão/interpreter permanece hardcoded
4. **`listen` não cobre plenamente interface:porta** → parse/build ainda não resolve `ip:porta` via diretiva `listen`

### Diagnóstico de não-bloqueio (2026-03-18)
- Estruturalmente o servidor está em modo não-bloqueante: sockets e pipes CGI em `O_NONBLOCK` + I/O dirigido por epoll.
- Resultado de carga informado (`siege -c1000 -r100`) reforça que o loop não bloqueia no cenário de resposta simples.
- O status "PARCIAL" no requisito de não-bloqueio ficou relacionado a **hardening de edge cases**, não a bloqueio estrutural atual.
- Hardening aplicado: `accept` em lote por evento e tratamento mais resiliente para `recv/send/read/write` negativos em fluxos non-blocking.

### Edge Cases de hardening (pendentes)
- `epoll_ctl` lança exceções (`runtime_error`) em falhas e pode encerrar o processo sem fallback de recuperação.
- Parser incremental atual cobre `Content-Length`; `Transfer-Encoding: chunked` ainda não é tratado.

**📋 Resumo por Categoria:**
- **Core do Servidor:** 5/8 completos
- **Métodos HTTP:** 3/3 implementados (versão básica)
- **Responses:** 1/3 funcionais
- **Config:** 6/11 completos
- **CGI:** 5/6 completos

---

## Requisitos do Subject (Cap. III)

### Core do Servidor
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 1 | Compilar com `-Wall -Wextra -Werror -std=c++98` | ✅ FEITO | Makefile corrigido e build principal (`make`) validada em 2026-03-18 |
| 2 | Arquivo de configuracao como argumento | ✅ FEITO | `main` carrega arquivo, valida e constrói config em runtime |
| 3 | Servidor nao-bloqueante | ✅ FEITO (com hardening pendente) | epoll + sockets/CGI non-blocking, parser incremental por conexão e loop bloqueante por evento |
| 4 | Usar apenas 1 poll/epoll para TODAS operacoes I/O | ✅ FEITO | Sockets de servidor/cliente e pipes de CGI passam pelo mesmo EpollManager |
| 5 | Nunca fazer read/write sem poll() | ✅ FEITO (PIPELINE REDE/CGI) | Leitura/escrita de sockets e pipes CGI é dirigida por eventos EPOLLIN/EPOLLOUT; hardening de I/O transitório aplicado |
| 6 | fork() apenas para CGI | ✅ FEITO | fork() so esta em CgiProcessExecutor.cpp |
| 7 | Servidor nunca deve crashar | ⏸️ A TESTAR | Shutdown gracioso por SIGINT/SIGTERM validado; ainda falta stress/chaos test longo |
| 8 | Ouvir em multiplas portas | ✅ FEITO | `Server` registra múltiplos sockets de listen no mesmo epoll e aceita conexões em portas distintas (validado em 8091/8092) |

### Metodos HTTP
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 9 | GET | ✅ FEITO | StaticFileHandler com root + URI, index, autoindex, MIME e 404/403 |
| 10 | POST | ✅ FEITO (BÁSICO) | Upload simplificado para arquivo/diretório, 201 e validação de allow_methods |
| 11 | DELETE | ✅ FEITO (BÁSICO) | remove arquivo com proteção de path traversal, 204/404/403 |

### Responses
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 12 | Classe HttpResponse | ✅ FEITO | HttpResponse.cpp/.hpp completos com serialize(), usado em connectionManager linha 67-72 |
| 13 | Paginas de erro padrao | ⚠️ PARCIAL | Existem respostas de erro HTML, mas a integração de `error_page` por config permanece pendente |
| 14 | Status codes corretos | ⚠️ PARCIAL | Códigos principais implementados; faltam redirect e casos avançados |

### Arquivo de Configuracao
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 15 | Lexer (tokenizacao) | ✅ FEITO | Lexer.cpp completo e testado |
| 16 | Parser (tokens -> AST) | ✅ FEITO | Parser.cpp completo, gera ASTNode tree |
| 17 | AST -> objeto Config | ✅ FEITO | ConfigBuilder gera HttpBlock/ServerBlock e main usa config em runtime |
| 18 | Definir interface:porta | ⚠️ PARCIAL | Multi-porta está funcional, mas o parse/build de `listen ip:porta` ainda não está completo no pipeline de config/runtime |
| 19 | Paginas de erro customizadas (por config) | ❌ NAO FEITO | Diretiva `error_page` ainda não está consolidada no pipeline conforme esperado pelo subject |
| 20 | Tamanho max do corpo (client_max_body_size) | ✅ FEITO | Validator passou a consumir limite configurado por `ServerBlock`; erros de excesso/mismatch retornam `413` |
| 21 | Rotas com metodos aceitos | ✅ FEITO (BÁSICO) | Prefix match por location + allow_methods em GET/POST/DELETE |
| 22 | Redirecionamento (return) | ✅ FEITO | `return /destino` (302 padrão) e `return <3xx> /destino` por location com header `Location` |
| 23 | Diretorio raiz (root) | ✅ FEITO | Resolução de path baseada em `root + URI` |
| 24 | Listagem de diretorios (autoindex) | ✅ FEITO | DirectoryLister integrado ao GET |
| 25 | Arquivo padrao (index) | ✅ FEITO | Fallback para `index` em diretórios |

### CGI
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 26 | Executar CGI baseado em extensao (.py, .php) | ⚠️ PARCIAL | Execução CGI funciona, mas decisão de extensão/interpreter ainda não consome `cgi_ext`/`cgi_path` da config |
| 27 | CgiEnvironment (variaveis CGI) | ✅ FEITO | CgiEnvironment.cpp buildFromRequest() completo, toEnvArray() OK |
| 28 | CgiResponse (parse output) | ✅ FEITO | CgiResponse parse headers + body + status |
| 29 | CgiHandler (orquestrador) | ✅ FEITO | CgiHandler.cpp/hpp completo, buildResponse() retorna HttpResponse |
| 30 | Integrar CGI no event loop | ✅ FEITO | `CgiOrchestrator` adiciona/remove fds CGI no epoll e processa resposta assíncrona |
| 31 | Timeout de CGI | ✅ FEITO | checkState() com SIGKILL |

---

## Pipeline Atual vs Pipeline Necessario

### O que existe (funcional):
```
Config File -> Lexer -> Tokens -> Parser -> AST -> ConfigBuilder -> HttpBlock/ServerBlock

Raw HTTP -> HttpRequestParser -> HttpRequest -> HttpRequestValidator -> ConnectionManager

HttpRequest -> CgiEnvironment -> CgiProcessExecutor -> CgiResponse -> HttpResponse (pipeline CGI)

Sockets -> epoll -> accept -> handleClientRead -> dispatch (CGI / GET / POST / DELETE) -> write
```

### O que falta para funcionar end-to-end:
```
✅ AST criado
✅ AST -> Config (ConfigBuilder)
     |
     v
✅ Config -> Server multi-porta (múltiplos listeners por processo)
     |
     v
✅ Request chega -> epoll ready
✅ parse HTTP (HttpRequestParser + Validator)
✅ Dispatcher básico (match de método + location por prefix)
     |
   +--> ✅ GET Handler:    FileReader + autoindex + index fallback
   +--> ✅ POST Handler:   upload simplificado + allow_methods
   +--> ✅ DELETE Handler: remoção de arquivo + allow_methods
     |
     v
✅ HttpResponse.serialize() -> write buffer
⚠️ Error pages custom por config ainda pendentes
```

---

## Proximos Passos Sugeridos (ordem de prioridade)

### ~~1. HttpResponse~~ ✅ JA EXISTE
- ✅ Classe completa em `src/domain/entities/HttpResponse.cpp`
- ✅ Metodos: setStatusCode, setHeader, setBody, serialize
- ✅ Usado no pipeline real (CGI + GET/POST/DELETE)

### 2. Consolidar Config em Runtime
- Aplicar `host` do arquivo de configuração no bootstrap do Server (hoje hardcoded)
- Preparar suporte para múltiplos `server` blocks/portas no mesmo processo
- Finalizar diretiva pendente (`error_page` custom por location/server)

### 3. Evoluir Router / Dispatcher
- Extrair decisão de rota/método de `ConnectionManager` para componente dedicado
- Unificar política de `Allow` e respostas 405

### 4. Hardening de Handlers HTTP
- POST: suportar `multipart/form-data` e `application/x-www-form-urlencoded` de forma completa
- DELETE: definir política para diretórios (proibir ou remover recursivo por config)
- GET/POST/DELETE: aplicar `error_page` custom quando configurado

### 5. Multiplas portas/server blocks
- ✅ Config retorna múltiplos `ServerBlock` em `HttpBlock.servers`
- ✅ `main.cpp` inicializa `Server` com vetor de server blocks
- ✅ `Server` adiciona todos os listen fds no mesmo `EpollManager`
- ✅ `ConnectionManager` vincula `listener_fd -> ServerBlock` e herda para `client_fd`, aplicando regras corretas por porta

---

## Bugs/Issues Conhecidos

| # | Arquivo | Issue | Severidade |
|---|---------|-------|------------|
| 1 | `ipAddr.cpp` | `isValidIp()` so verifica non-empty, sem validacao real | MEDIA |
| 2 | `TokenResult.cpp` | Destructor nao deleta tokens (comentado) | BAIXA (leak) |
| 3 | `HttpRequestValidator.cpp` | ✅ Resolvido em 2026-03-19: validação com limite configurável + erro tipado com `413` para excesso/mismatch | - |
| 4 | `connectionManager.cpp` | Dispatcher HTTP ainda acoplado no connection manager (SRP fraco) | MEDIA |
| 5 | `connectionManager.cpp` | ✅ Resolvido em 2026-03-19: associação `listener_fd/client_fd -> ServerBlock` aplicada no parse/validator/handlers | - |
| 6 | `application/CGI/CgiRouteResolver.cpp` | Roteamento de CGI foi desacoplado do `ConnectionManager`, mas ainda baseado em extensão fixa (.py/.php), sem usar `cgi_path`/`cgi_ext` | MEDIA |
| 7 | `interfaces/old_main.cpp` | Codigo morto | BAIXA |
| 8 | `HttpResponseBuilders.hpp` / handlers HTTP | Suporte de `error_page` por config ainda pendente no pipeline final | MEDIA |
| 9 | `ConfigBuilder.cpp` / `ServerBlock.hpp` | `listen` ainda tratado como porta simples; falta suporte robusto a interface:porta conforme subject | MEDIA |

### Conflitos Resolvidos (2026-03-18)

1. Makefile: continuação de linha em `APPLICATION_SRCS` corrigida para restaurar parsing do make.
2. main.cpp: removido fluxo duplicado e referências inválidas (`ConfigBuilder`, `astRoot`, construtor antigo de `Server`).
3. Redirect por `return` habilitado em handlers HTTP (GET/POST/DELETE).
4. Integração de pipes CGI no epoll validada em cenário concorrente (`slow.py` + GET estático).
5. `make test` passou a executar `test_cgi` via target oficial (sem execução manual separada).
6. Validação HTTP passou a usar erro tipado no use case + `413` consistente para `client_max_body_size` e `Content-Length mismatch`.
7. `HttpRequestValidationIssue` passou a reutilizar `HttpStatusCode` (sem duplicar literais mágicos `400/413`).
8. `ConnectionManager` passou a propagar config correta por conexão (`listener_fd -> client_fd -> ServerBlock`), validado com limites distintos em portas diferentes.
9. Loop principal passou a usar `EPOLLIN` de forma consistente e `epoll_wait` bloqueante (`waitForEvents()`), com encerramento por sinal (SIGINT/SIGTERM).
10. Parsing HTTP virou incremental por conexão com buffer persistente e montagem completa por `Content-Length`.
12. Roteamento de CGI extraído de `ConnectionManager` para `application/CGI/CgiRouteResolver` (melhora de separação de responsabilidades, sem mudança funcional).

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

**Prioridade 1:** Implementar suporte completo a `error_page` no pipeline de resposta

**Prioridade 2:** Consumir `cgi_path`/`cgi_ext` no pipeline de CGI (resolver + execução)

**Prioridade 3:** Fechar suporte de `listen interface:porta` no parse/build/runtime

**Prioridade 4:** Expandir parser incremental para `Transfer-Encoding: chunked`

**Prioridade 5:** Endurecer tratamento de falhas de `epoll_ctl` e adicionar stress/chaos tests
