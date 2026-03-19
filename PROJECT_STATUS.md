# Webserv - Status do Projeto

**Última atualização:** 2026-03-18

## Atualização de Hoje (2026-03-18)

- Erro crítico de sintaxe no Makefile resolvido (`recipe commences before first target` na linha 88).
- Conflito lógico em `src/interfaces/cli/main.cpp` resolvido (fluxos de bootstrap duplicados/remanescentes de desenvolvimento).
- Build principal validada com sucesso: `make` conclui link de `webserver`.
- Testes atuais validados com sucesso: `make test` (HttpRequest, HttpRequestValidation e ParseAndValidateHttpRequestUseCase).
- Suporte a HTTP/1.1 habilitado no validador e testes atualizados para aceitar 1.1.
- Configuração de `root` ajustada para diretório existente (`./www/`) e smoke test validado em runtime.
- Validação runtime (HTTP/1.1): `GET /` -> 200, `GET /tours` -> 200, `GET /cgi-bin/hello.py` -> 200.
- Diretiva `return` implementada por location (`/red` retorna `302` com header `Location: /tours`).
- Integração CGI no event loop consolidada (pipes de CGI registrados no epoll com tratamento EPOLLIN/EPOLLOUT).
- Validação de concorrência: `GET /cgi-bin/slow.py` em execução e `GET /` respondendo em paralelo sem bloquear o loop.
- Pendências funcionais do subject continuam, especialmente multi-porta/multi-server block e `error_page` custom por config.

## 📊 Resumo Executivo

### Progresso Geral: ~68% (21/31 requisitos obrigatórios)

**✅ COMPLETO (68%):**
- Infraestrutura de rede (epoll, sockets non-blocking)
- Parsing de configuração (Lexer → Parser → AST)
- Build de Config em runtime (AST -> HttpBlock/ServerBlock)
- Parsing de HTTP requests (HttpRequest + HttpRequestValidator)
- HttpResponse com serialização
- GET estático (arquivo, index, autoindex, 403/404)
- POST básico de upload (criação de arquivo, 201, allow_methods)
- DELETE básico (remoção de arquivo, 204/403/404, allow_methods)
- CGI básico (CgiHandler, CgiEnvironment, CgiProcessExecutor)

**🔴 BLOQUEIOS CRÍTICOS:**
1. **Suporte multi-porta/server blocks** → servidor ainda roda com um server socket por processo
2. **client_max_body_size inconsistente** → validador ainda usa limite fixo de 1MB
3. **Config incompleta para subject** → páginas de erro custom por rota pendentes
4. **POST ainda simplificado** → sem multipart/form-data

**📋 Resumo por Categoria:**
- **Core do Servidor:** 4/8 completos
- **Métodos HTTP:** 3/3 implementados (versão básica)
- **Responses:** 2/3 funcionais
- **Config:** 8/11 completos
- **CGI:** 6/6 completos

---

## Requisitos do Subject (Cap. III)

### Core do Servidor
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 1 | Compilar com `-Wall -Wextra -Werror -std=c++98` | ✅ FEITO | Makefile corrigido e build principal (`make`) validada em 2026-03-18 |
| 2 | Arquivo de configuracao como argumento | ✅ FEITO | `main` carrega arquivo, valida e constrói config em runtime |
| 3 | Servidor nao-bloqueante | ⚠️ PARCIAL | epoll + sockets non-blocking + dispatch básico; falta hardening de edge cases |
| 4 | Usar apenas 1 poll/epoll para TODAS operacoes I/O | ✅ FEITO | Sockets de servidor/cliente e pipes de CGI passam pelo mesmo EpollManager |
| 5 | Nunca fazer read/write sem poll() | ✅ FEITO (PIPELINE REDE/CGI) | Leitura/escrita de sockets e pipes CGI é dirigida por eventos EPOLLIN/EPOLLOUT |
| 6 | fork() apenas para CGI | ✅ FEITO | fork() so esta em CgiProcessExecutor.cpp |
| 7 | Servidor nunca deve crashar | ⏸️ A TESTAR | Falta stress test, mas protecoes basicas existem |
| 8 | Ouvir em multiplas portas | ❌ NAO FEITO | Server.cpp linha 5 aceita 1 Port/IpAddr, sem suporte multi-porta |

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
| 13 | Paginas de erro padrao | ⚠️ PARCIAL | ErrorPageGenerator integrado em GET/POST/DELETE; falta `error_page` custom por config |
| 14 | Status codes corretos | ⚠️ PARCIAL | Códigos principais implementados; faltam redirect e casos avançados |

### Arquivo de Configuracao
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 15 | Lexer (tokenizacao) | ✅ FEITO | Lexer.cpp completo e testado |
| 16 | Parser (tokens -> AST) | ✅ FEITO | Parser.cpp completo, gera ASTNode tree |
| 17 | AST -> objeto Config | ✅ FEITO | ConfigBuilder gera HttpBlock/ServerBlock e main usa config em runtime |
| 18 | Definir interface:porta | ⚠️ PARCIAL | `listen`/`host` são aplicados a partir da config; ainda sem suporte multi-server/multi-porta |
| 19 | Paginas de erro customizadas (por config) | ❌ NAO FEITO | Sem Config para mapear error_page |
| 20 | Tamanho max do corpo (client_max_body_size) | ⚠️ PARCIAL | POST handler usa config; validator global ainda hardcoded em 1MB |
| 21 | Rotas com metodos aceitos | ✅ FEITO (BÁSICO) | Prefix match por location + allow_methods em GET/POST/DELETE |
| 22 | Redirecionamento (return) | ✅ FEITO | `return /destino` (302 padrão) e `return <3xx> /destino` por location com header `Location` |
| 23 | Diretorio raiz (root) | ✅ FEITO | Resolução de path baseada em `root + URI` |
| 24 | Listagem de diretorios (autoindex) | ✅ FEITO | DirectoryLister integrado ao GET |
| 25 | Arquivo padrao (index) | ✅ FEITO | Fallback para `index` em diretórios |

### CGI
| # | Requisito | Status | Notas |
|---|-----------|--------|-------|
| 26 | Executar CGI baseado em extensao (.py, .php) | ✅ FEITO | CgiProcessExecutor.cpp com fork/execve/pipes linha 51 |
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
❌ Config -> Server multi-porta (atualmente 1 porta por processo)
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
- Finalizar diretiva pendente (`error_page` custom por location)

### 3. Evoluir Router / Dispatcher
- Extrair decisão de rota/método de `ConnectionManager` para componente dedicado
- Unificar política de `Allow` e respostas 405

### 4. Hardening de Handlers HTTP
- POST: suportar `multipart/form-data` e `application/x-www-form-urlencoded` de forma completa
- DELETE: definir política para diretórios (proibir ou remover recursivo por config)
- GET/POST/DELETE: aplicar `error_page` custom quando configurado

### 5. Multiplas portas/server blocks
- Config retornar vector<ServerConfig>
- main.cpp criar vector<Server*>, cada um com sua porta
- Adicionar todos ServerSocket fds no mesmo EpollManager
- connectionManager identificar qual Config usar por fd

---

## Bugs/Issues Conhecidos

| # | Arquivo | Issue | Severidade |
|---|---------|-------|------------|
| 1 | `ipAddr.cpp` | `isValidIp()` so verifica non-empty, sem validacao real | MEDIA |
| 2 | `TokenResult.cpp` | Destructor nao deleta tokens (comentado) | BAIXA (leak) |
| 3 | `HttpRequestValidator.cpp` | Limite de `Content-Length` fixo (1MB), ignora `client_max_body_size` por server | ALTA |
| 4 | `connectionManager.cpp` | Dispatcher HTTP ainda acoplado no connection manager (SRP fraco) | MEDIA |
| 5 | `server.cpp` | Instancia de server ainda limitada a 1 socket/porta por processo | ALTA |
| 6 | `connectionManager.cpp` | Roteamento de CGI ainda baseado em extensão fixa (.py/.php), sem usar integralmente `cgi_path`/`cgi_ext` da config | MEDIA |
| 7 | `interfaces/old_main.cpp` | Codigo morto | BAIXA |
| 8 | `ErrorPageGenerator` | Existe mas nunca usado no pipeline real | MEDIA |

### Conflitos Resolvidos (2026-03-18)

1. Makefile: continuação de linha em `APPLICATION_SRCS` corrigida para restaurar parsing do make.
2. main.cpp: removido fluxo duplicado e referências inválidas (`ConfigBuilder`, `astRoot`, construtor antigo de `Server`).
3. Redirect por `return` habilitado em handlers HTTP (GET/POST/DELETE).
4. Integração de pipes CGI no epoll validada em cenário concorrente (`slow.py` + GET estático).

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

**Prioridade 1:** Suporte multi-porta + múltiplos server blocks
**Prioridade 2:** Unificar `client_max_body_size` no validator/handler (retorno 413 consistente)
**Prioridade 3:** Implementar `error_page` custom por location e alinhar roteamento CGI com config (`cgi_path`/`cgi_ext`)
