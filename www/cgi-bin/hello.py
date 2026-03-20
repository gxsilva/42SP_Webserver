#!/usr/bin/env python3
import html
import os
import sys


def read_body():
    raw_length = os.environ.get("CONTENT_LENGTH", "")
    if not raw_length:
        return ""

    try:
        length = int(raw_length)
    except ValueError:
        return ""

    if length <= 0:
        return ""
    return sys.stdin.read(length)


method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
query = os.environ.get("QUERY_STRING", "")
body = read_body()

sys.stdout.write("Status: 200 OK\r\n")
sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
sys.stdout.write("X-CGI-Test: hello.py\r\n")
sys.stdout.write("\r\n")

sys.stdout.write("<!DOCTYPE html>\n")
sys.stdout.write("<html><body>\n")
sys.stdout.write("<h1>CGI hello.py is working</h1>\n")
sys.stdout.write("<p>Method: {}</p>\n".format(html.escape(method)))
sys.stdout.write("<p>Query: {}</p>\n".format(html.escape(query)))

if body:
    preview = body[:500]
    if len(body) > 500:
        preview += "... [truncated]"
    sys.stdout.write("<p>Body:</p><pre>{}</pre>\n".format(html.escape(preview)))

sys.stdout.write("<p>For full diagnostics use <a href=\"/cgi-bin/cgi_diagnostic.py\">cgi_diagnostic.py</a>.</p>\n")
sys.stdout.write("</body></html>\n")
