# Copilot Instructions – Webserv

## Project Context

This is a 42 School project that implements an HTTP/1.1 web server from scratch in **C++98**.
Full project requirements are described in `webserv_context.md`.

The architecture follows **Domain-Driven Design (DDD)** principles.
The layer structure is not yet defined — when suggesting organization, Copilot should propose DDD-aligned separations but never impose a fixed directory layout.

**Object Calisthenics** principles should be kept in mind and applied when they genuinely improve clarity — never forced.

---

## Language

- Standard: **C++98 only** — no C++11 or later features (`auto`, lambdas, `nullptr`, smart pointers, range-for)
- Compile flags: `-Wall -Wextra -Werror -Wshadow -std=c++98`
- Prefer C++ headers: `<cstring>`, `<cstdlib>`, `<cstdio>` over their C equivalents
- No external libraries, no Boost

---

## Priorities — What Copilot Should Always Consider

### 1. Object Calisthenics (when applicable)
Apply these rules when they improve cohesion and readability:
- One level of indentation per method
- No `else` after `return`
- Wrap primitives that carry meaning (e.g., port, URI, HTTP method) in value objects
- Keep classes and methods small and focused on a single responsibility
- Avoid long method chains

### 2. Strict Separation of Responsibilities
- Each class must have one clear reason to change
- Never mix domain logic (HTTP rules, config semantics) with infrastructure (syscalls, sockets, file I/O)
- Parsers, handlers, and I/O must live in separate concerns
- When in doubt, ask: "would this class need to change if I switched the transport layer?" — if yes, split it

### 3. Memory Safety — No Unnecessary Allocation
- Every `new` must have a clear, paired `delete`
- Prefer stack allocation; use heap only when object lifetime demands it
- Destructors must release all owned resources (memory, file descriptors, sockets)
- Validate all external input before processing — requests, config, CGI output
- The server must never crash under any circumstance

---

## Non-Blocking I/O — Hard Constraints

These are project rules. Violations result in a grade of 0.

- Use **exactly one `poll()`** (or equivalent) for all I/O — including `listen`
- **Never** call `read`, `recv`, `write`, or `send` without first confirming readiness via `poll()`
- `poll()` must watch both **read and write** simultaneously
- **Never** use `errno` after read/write to adjust server behavior
- `fork()` is only allowed for CGI execution

---

## What Copilot Should Avoid

- C++11 or later syntax and features
- Calling I/O functions outside of the poll event loop
- Mixing syscalls into domain/business logic classes
- Global mutable state
- Large monolithic classes or methods
- Checking `errno` after I/O operations to drive server decisions

---

## Reference

- Full project requirements: `webserv_context.md`
- Behavior reference: compare against **NGINX** when in doubt about HTTP semantics