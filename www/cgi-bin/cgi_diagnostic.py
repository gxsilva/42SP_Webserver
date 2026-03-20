#!/usr/bin/env python3
import html
import json
import os
import sys
import time
from urllib.parse import parse_qs


STATUS_TEXT = {
    200: "OK",
    201: "Created",
    204: "No Content",
    400: "Bad Request",
    403: "Forbidden",
    404: "Not Found",
    405: "Method Not Allowed",
    413: "Content Too Large",
    500: "Internal Server Error",
    502: "Bad Gateway",
    504: "Gateway Timeout",
}


def h(value):
    return html.escape(str(value), quote=True)


def parse_status(query):
    raw = query.get("status", ["200"])[0]
    try:
        code = int(raw)
    except ValueError:
        return 400

    if code < 100 or code > 599:
        return 400
    return code


def read_request_body():
    length_raw = os.environ.get("CONTENT_LENGTH", "")
    if not length_raw:
        return ""

    try:
        length = int(length_raw)
    except ValueError:
        return ""

    if length <= 0:
        return ""
    return sys.stdin.read(length)


def collect_http_headers():
    headers = []
    for key in sorted(os.environ.keys()):
        if key.startswith("HTTP_"):
            header_name = key[5:].replace("_", "-")
            headers.append((header_name, os.environ.get(key, "")))
    return headers


def render_table(rows):
    parts = ["<table><tbody>"]
    for left, right in rows:
        parts.append("<tr><th>{}</th><td>{}</td></tr>".format(h(left), h(right)))
    parts.append("</tbody></table>")
    return "".join(parts)


def build_html_response(method, query_string, query, body, status_code, elapsed_ms):
    env_rows = [
        ("REQUEST_METHOD", os.environ.get("REQUEST_METHOD", "")),
        ("REQUEST_URI", os.environ.get("REQUEST_URI", "")),
        ("SCRIPT_NAME", os.environ.get("SCRIPT_NAME", "")),
        ("QUERY_STRING", query_string),
        ("CONTENT_TYPE", os.environ.get("CONTENT_TYPE", "")),
        ("CONTENT_LENGTH", os.environ.get("CONTENT_LENGTH", "")),
        ("SERVER_PROTOCOL", os.environ.get("SERVER_PROTOCOL", "")),
        ("SERVER_NAME", os.environ.get("SERVER_NAME", "")),
        ("SERVER_PORT", os.environ.get("SERVER_PORT", "")),
        ("REMOTE_ADDR", os.environ.get("REMOTE_ADDR", "")),
    ]

    query_rows = []
    for key in sorted(query.keys()):
        query_rows.append((key, ", ".join(query.get(key, []))))
    if not query_rows:
        query_rows.append(("(none)", ""))

    header_rows = collect_http_headers()
    if not header_rows:
        header_rows.append(("(none)", ""))

    body_preview = body
    if len(body_preview) > 2000:
        body_preview = body_preview[:2000] + "\n... [truncated]"

    status_line = "{} {}".format(status_code, STATUS_TEXT.get(status_code, "Custom Status"))

    return """<!DOCTYPE html>
<html lang=\"en\">
<head>
  <meta charset=\"UTF-8\" />
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />
  <title>CGI Diagnostic</title>
  <style>
    body {{
      margin: 0;
      padding: 24px;
      font-family: Arial, Helvetica, sans-serif;
      color: #111827;
      background: #f3f6fb;
    }}
    .wrap {{ max-width: 980px; margin: 0 auto; }}
    .card {{
      background: #ffffff;
      border: 1px solid #dbe2ee;
      border-radius: 10px;
      padding: 14px;
      margin-bottom: 12px;
    }}
    h1, h2 {{ margin: 0 0 10px; }}
    p {{ margin: 6px 0; }}
    code {{ background: #eef2f7; padding: 2px 6px; border-radius: 6px; }}
    table {{ width: 100%; border-collapse: collapse; font-size: 14px; }}
    th, td {{
      border: 1px solid #e4e8f0;
      text-align: left;
      padding: 8px;
      vertical-align: top;
    }}
    th {{ width: 35%; background: #f8fafc; }}
    pre {{
      margin: 0;
      background: #0f172a;
      color: #e2e8f0;
      border-radius: 8px;
      padding: 10px;
      white-space: pre-wrap;
      word-break: break-word;
    }}
    .ok {{ color: #0f766e; }}
  </style>
</head>
<body>
  <div class=\"wrap\">
    <div class=\"card\">
      <h1>CGI Diagnostic Endpoint</h1>
      <p class=\"ok\"><strong>Status:</strong> {status_line}</p>
      <p><strong>Method:</strong> <code>{method}</code></p>
      <p><strong>Elapsed:</strong> {elapsed_ms} ms</p>
      <p><strong>Try:</strong></p>
      <p><code>/cgi-bin/cgi_diagnostic.py?status=201</code></p>
      <p><code>/cgi-bin/cgi_diagnostic.py?status=400&name=webserv&name=cgi</code></p>
      <p><code>POST /cgi-bin/cgi_diagnostic.py?status=200</code> with a body</p>
    </div>

    <div class=\"card\">
      <h2>CGI Environment</h2>
      {env_table}
    </div>

    <div class=\"card\">
      <h2>Query Parameters</h2>
      {query_table}
    </div>

    <div class=\"card\">
      <h2>Request Headers (HTTP_*)</h2>
      {header_table}
    </div>

    <div class=\"card\">
      <h2>Request Body</h2>
      <p>Raw body length: <code>{body_len}</code></p>
      <pre>{body_preview}</pre>
    </div>
  </div>
</body>
</html>
""".format(
        status_line=h(status_line),
        method=h(method),
        elapsed_ms=h(elapsed_ms),
        env_table=render_table(env_rows),
        query_table=render_table(query_rows),
        header_table=render_table(header_rows),
        body_len=h(len(body)),
        body_preview=h(body_preview),
    )


def main():
    start = time.time()
    method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
    query_string = os.environ.get("QUERY_STRING", "")
    query = parse_qs(query_string, keep_blank_values=True)
    status_code = parse_status(query)
    body = read_request_body()
    elapsed_ms = int((time.time() - start) * 1000)
    response_format = query.get("format", ["html"])[0].lower()

    sys.stdout.write("Status: {} {}\r\n".format(status_code, STATUS_TEXT.get(status_code, "Custom Status")))
    sys.stdout.write("X-CGI-Test: cgi_diagnostic.py\r\n")
    sys.stdout.write("X-CGI-Method: {}\r\n".format(method))

    if response_format == "json":
        payload = {
            "status": status_code,
            "status_text": STATUS_TEXT.get(status_code, "Custom Status"),
            "method": method,
            "elapsed_ms": elapsed_ms,
            "query_string": query_string,
            "query": query,
            "content_type": os.environ.get("CONTENT_TYPE", ""),
            "content_length": os.environ.get("CONTENT_LENGTH", ""),
            "body_length": len(body),
            "body": body,
        }
        sys.stdout.write("Content-Type: application/json; charset=utf-8\r\n")
        sys.stdout.write("\r\n")
        sys.stdout.write(json.dumps(payload, indent=2, sort_keys=True))
        return

    response_body = build_html_response(method, query_string, query, body, status_code, elapsed_ms)
    sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
    sys.stdout.write("\r\n")
    sys.stdout.write(response_body)


if __name__ == "__main__":
    main()
