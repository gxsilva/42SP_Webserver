# 42SP_Webserver
42 SP | Web Server Project — HTTP/1.1 server implemented from scratch in **C++98**.

---

## Project Status

> **Phase:** Config pipeline complete — Lexer → Parser → AST ✅  
> **Next:** Config semantic validation → Server bootstrap → HTTP request handling

The project is being built incrementally following a compiler-style pipeline:

```
config.conf → Lexer → [Token] → Parser → AST → Semantic Validator → ServerConfig → HTTP Server
```

### Current Pipeline State

| Stage              | Status      | Description                                         |
|--------------------|-------------|-----------------------------------------------------|
| Lexer              | ✅ Done      | Tokenizes `.conf` files into typed tokens           |
| Parser             | ✅ Done      | Builds an AST from tokens (directives + blocks)     |
| AST Nodes          | ✅ Done      | `ASTRoot`, `ASTBlock`, `ASTDirective`, `ASTValue`   |
| Semantic Validator | 🔲 Pending  | Validates config semantics against HTTP/1.1 rules   |
| ServerConfig       | 🔲 Pending  | Domain model for virtual hosts, routes, CGI         |
| HTTP Server        | 🔲 Pending  | Non-blocking I/O loop with `poll()`                 |
| CGI Handler        | 🔲 Pending  | `fork()` + pipe-based CGI execution                 |

---

## Architecture

The project follows **Domain-Driven Design (DDD)** layering:

```
src/
├── application/        # Use cases and port interfaces (ILogger)
├── domain/             # Core logic — entities, services, value objects, errors
│   ├── entities/       # Token, SourceLocation, AST nodes
│   ├── services/       # Lexer, Parser
│   ├── errors/         # CompilerError, ErrorList
│   └── value_objects/  # TokenType, ASTNodeType, ErrorCode, ErrorSeverity
├── infrastructure/     # I/O, logging, result types
│   ├── common/         # Result wrappers (LexerResult, TokenResult, ASTResult)
│   ├── io/             # FileReader, FileValidator
│   └── logging/        # Logger (ILogger implementation)
└── interfaces/         # CLI entry point (main.cpp)
```

### Key Design Patterns

**Result / Either pattern** — all fallible operations return a typed result object instead of throwing exceptions or returning raw pointers:

```cpp
TokenResult res = CompileSourceFile::execute(argv[1], &logger);
if (res.isErr()) { res.error().formatAllErrors(); return 1; }
std::vector<Token>* tokens = res.unwrap(); // transfers ownership
```

Result types: [`LexerResult`](src/infrastructure/common/LexerResult.hpp), [`TokenResult`](src/infrastructure/common/TokenResult.hpp), [`ASTResult`](src/infrastructure/common/ASTResult.hpp) — all inherit from [`ResultBase`](src/infrastructure/common/ResultBase.hpp).

**Static Factory Methods** on [`CompilerError`](src/domain/errors/CompilerError.hpp) produce typed, contextual errors (file errors, parse errors, lexer errors) with optional source location, hints, and notes.

---

## Hard Constraints (grade = 0 if violated)

- Single `poll()` loop for **all** I/O — including `listen`
- No `read`/`write`/`recv`/`send` outside poll readiness check
- No `errno` usage to drive server decisions after I/O
- `fork()` only for CGI

---

## Makefile Commands

| Target                  | Description                                                       |
|-------------------------|-------------------------------------------------------------------|
| `all`                   | Builds the project                                                |
| `clean`                 | Removes object files and dependency files                         |
| `fclean`                | Full cleanup — removes all build artifacts including the binary   |
| `re`                    | `fclean` + `all`                                                  |
| `format`                | Runs `clang-format` on all `.cpp` and `.hpp` files                |
| `tidy`                  | Runs `clang-tidy` using `compile_commands.json`                   |
| `compile_commands_json` | Generates `compile_commands.json` via `bear`                      |
| `check-tools`           | Verifies required tools are installed                             |
| `clean_logs`            | Removes all log files from `./` and `./log/`                      |

---

## Stack & Tooling

| Concern        | Tool / Standard            |
|----------------|----------------------------|
| Language       | C++98 (`-std=c++98`)       |
| Warnings       | `-Wall -Wextra -Werror -Wshadow` |
| Formatter      | `clang-format` (LLVM style)|
| Static Analysis| `clang-tidy` + `bear`      |
| Git hooks      | `pre-commit` + custom `commit-msg` hook |