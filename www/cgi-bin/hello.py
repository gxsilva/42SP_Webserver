#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html")
print("Status: 200 OK")
print()
print("<html><body>")
print("<h1>CGI Works!</h1>")
print("<p>Method: {}</p>".format(os.environ.get("REQUEST_METHOD", "unknown")))
print("<p>Query: {}</p>".format(os.environ.get("QUERY_STRING", "")))

if os.environ.get("REQUEST_METHOD") == "POST":
    body = sys.stdin.read()
    print("<p>Body: {}</p>".format(body))

print("</body></html>")
