# Fluxo atual do projeto (estado até agora)

Este documento descreve o fluxo **como o código está hoje** e o fluxo-alvo para chegar ao comportamento esperado do webserver.

## 1) Fluxo de configuração

### O que já existe

```text
Arquivo .conf
  -> FileReader / FileValidator
  -> Lexer
  -> Parser
  -> AST (árvore sintática)
```

### O que ainda falta

```text
AST
  -> Builder/Mapper de Config
  -> objeto Config executável
  -> bootstrap do servidor baseado em config real
```

Sem essa etapa, o servidor não usa totalmente as diretivas do arquivo de configuração.

## 2) Fluxo de rede (runtime atual)

```text
ServerSocket (listen)
  -> EpollManager (epoll_wait)
  -> ConnectionManager
      - EPOLLIN: lê dados do cliente
      - EPOLLOUT: envia dados do buffer
```

No estado atual, após leitura, a resposta enviada ainda é de placeholder (`Hello, World!`).

## 3) Fluxo HTTP de domínio (já implementado, mas não acoplado totalmente ao runtime)

```text
Raw HTTP request
  -> HttpRequestParser
  -> HttpRequest
  -> HttpRequestValidator
```

Esse fluxo existe e tem testes de unidade, mas ainda não está conduzindo o response final no caminho principal de rede.

## 4) Fluxo CGI (módulos já prontos)

### Componentes
- `CgiEnvironment`: monta variáveis CGI a partir do `HttpRequest`.
- `CgiProcessExecutor`: cria pipes, faz `fork/execve`, escreve body no stdin do CGI, lê stdout do CGI, controla timeout e cleanup.
- `CgiResponse`: parseia output CGI (headers/body/status).
- `CgiHandler`: orquestra request -> gateway CGI -> `HttpResponse`.

### Fluxo interno de CGI

```text
HttpRequest
  -> CgiEnvironment
  -> CgiProcessExecutor.start()
      -> pipe + fork + execve
      -> onWriteReady() para body
      -> onReadReady() para output
      -> checkState() para timeout/finalização
  -> CgiResponse.parse(output)
  -> HttpResponse final
```

### Gap atual

Esse fluxo ainda não está ligado ao loop principal do servidor. Ou seja, o CGI funciona no módulo, mas ainda não participa do caminho end-to-end de uma conexão real.

## 5) Fluxo alvo (próxima versão funcional)

```text
Cliente conecta
  -> epoll (readiness)
  -> ler request
  -> parse + validate
  -> router (location/método/config)
      -> GET handler
      -> POST handler (inclui CGI quando aplicável)
      -> DELETE handler
  -> HttpResponse
  -> serializar e enviar
  -> fechar ou manter conexão conforme headers
```

Para CGI no fluxo alvo:

```text
POST/GET roteado para CGI
  -> registrar pipes do CGI no epoll
  -> processar escrita/leitura por eventos
  -> timeout controlado
  -> montar HttpResponse com resultado do CGI
```

## 6) Situação de conformidade (snapshot)

- `fork()` apenas para CGI: OK.
- Base non-blocking com `epoll`: OK na rede principal.
- Integração completa de CGI no mesmo loop de eventos: pendente.
- Pipeline end-to-end (request real -> handler real -> response real): pendente.

## 7) Leitura rápida de maturidade

- **Camadas de domínio e infraestrutura**: bem encaminhadas.
- **Orquestração de aplicação**: parcialmente pronta.
- **Integração final**: principal foco das próximas entregas.
