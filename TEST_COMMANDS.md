# Test Commands (curl + siege)

This file provides practical commands to test the behavior configured in `config.conf`.

## 1) Start the server

```bash
make
./webserv config.conf
```

Use another terminal for the commands below.

## 2) Quick health checks

```bash
curl -i http://127.0.0.1:8080/
curl -i http://127.0.0.1:8081/
```

Expected: `HTTP/1.1 200 OK` for both.

## 3) Route and method tests (port 8080)

### Root (`/`) allows GET, POST, DELETE

```bash
curl -i -X GET http://127.0.0.1:8080/
curl -i -X POST http://127.0.0.1:8080/ -d "ping=root"
curl -i -X DELETE http://127.0.0.1:8080/
```

Expected: methods are allowed (status may vary by implementation, but should not be `405`).

### Uploads (`/uploads`) allows GET, POST, DELETE and autoindex on

```bash
curl -i http://127.0.0.1:8080/uploads/
curl -i -X POST http://127.0.0.1:8080/uploads/new_file.txt --data-binary "hello upload"
curl -i -X DELETE http://127.0.0.1:8080/uploads/sample_delete_me.txt
```

Expected:
- `GET /uploads/` returns directory listing (`200`).
- `POST` accepted (usually `2xx`/`201`).
- `DELETE` accepted for existing file.

### Readonly (`/readonly`) allows GET only and autoindex on

```bash
curl -i http://127.0.0.1:8080/readonly/
curl -i http://127.0.0.1:8080/readonly/info.txt
curl -i -X POST http://127.0.0.1:8080/readonly/info.txt -d "should fail"
curl -i -X DELETE http://127.0.0.1:8080/readonly/info.txt
```

Expected:
- `GET` works (`200`).
- `POST` and `DELETE` return `405 Method Not Allowed`.

### Error pages folder (`/error_pages`) allows GET only

```bash
curl -i http://127.0.0.1:8080/error_pages/
curl -i http://127.0.0.1:8080/error_pages/404.html
curl -i -X POST http://127.0.0.1:8080/error_pages/404.html -d "x"
```

Expected: `GET` works; `POST` returns `405`.

### Redirect (`/go-home`)

```bash
curl -i http://127.0.0.1:8080/go-home
curl -i -L http://127.0.0.1:8080/go-home
```

Expected:
- First command shows `302` with `Location: /`.
- Second command follows redirect and returns home page.

## 4) CGI tests (`/cgi-bin`)

Configured with:
- `cgi_path /usr/bin/python3`
- `cgi_ext .py`
- Allowed methods: `GET`, `POST`

```bash
curl -i http://127.0.0.1:8080/cgi-bin/hello.py
curl -i "http://127.0.0.1:8080/cgi-bin/cgi_diagnostic.py?name=webserv"
curl -i -X POST http://127.0.0.1:8080/cgi-bin/hello.py -d "msg=from_post"
curl -i -X DELETE http://127.0.0.1:8080/cgi-bin/hello.py
```

Expected:
- `GET` and `POST` execute CGI and return `2xx`.
- `DELETE` returns `405`.

Optional timeout behavior check:

```bash
curl -i http://127.0.0.1:8080/cgi-bin/slow.py
```

Expected: depends on your CGI timeout implementation (`200` if it finishes in time, or timeout-related error otherwise).

## 5) Body size limit test (`client_max_body_size 2k`)

Generate payload bigger than 2KB and POST it:

```bash
head -c 3000 /dev/zero | tr '\0' 'A' | curl -i -X POST http://127.0.0.1:8080/uploads/big.txt --data-binary @-
```

Expected: `413 Payload Too Large`.

## 6) Not found and custom error pages

```bash
curl -i http://127.0.0.1:8080/does-not-exist
curl -i -X POST http://127.0.0.1:8080/readonly/info.txt -d "x"
```

Expected:
- Missing resource returns `404` (using configured error page).
- Method violation returns `405` (using configured error page).

## 7) Listener on port 8081

Port `8081` has root `./www/listener/` and allows `GET`, `POST`, `DELETE` on `/`.

```bash
curl -i http://127.0.0.1:8081/
curl -i -X POST http://127.0.0.1:8081/ -d "listener test"
curl -i -X DELETE http://127.0.0.1:8081/
```

Expected: methods are allowed (not `405`).

## 8) Benchmark with siege

Install if needed (Ubuntu/Debian):

```bash
sudo apt-get update && sudo apt-get install -y siege
```

### Quick single-endpoint benchmark

```bash
siege -c 20 -t 30S http://127.0.0.1:8080/
```

### Benchmark static + autoindex + CGI mix

Create URL list:

```bash
cat > /tmp/webserv_urls.txt << 'EOF'
http://127.0.0.1:8080/
http://127.0.0.1:8080/uploads/
http://127.0.0.1:8080/readonly/
http://127.0.0.1:8080/error_pages/404.html
http://127.0.0.1:8080/cgi-bin/hello.py
http://127.0.0.1:8081/
EOF
```

Run mixed benchmark:

```bash
siege -c 30 -r 20 -f /tmp/webserv_urls.txt
```

### Stress CGI specifically

```bash
siege -c 10 -t 30S "http://127.0.0.1:8080/cgi-bin/hello.py"
```

## 9) Helpful one-liners

Only show status code:

```bash
curl -s -o /dev/null -w "%{http_code}\n" http://127.0.0.1:8080/
```

Show redirect target:

```bash
curl -s -o /dev/null -D - http://127.0.0.1:8080/go-home | grep -i '^Location:'
```
