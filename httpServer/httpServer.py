#!/usr/bin/env python3
# --*-- coding: utf-8 --*--

"""
Simple HTTP server in python3.
Usage::
    ./httpServer [<port>]
Send a GET request::
    curl http://localhost
Send a HEAD request::
    curl -I http://localhost
Send a POST request::
    curl -d "foo=bar&bin=baz" http://localhost
"""

from http.server import BaseHTTPRequestHandler, HTTPServer
import re
import subprocess

def parse_row_data(data):
    data = str(data)
    pattern = r'.+name="path"(.+)--boundary.+name="num_lines"(.+)--boundary'
    res = re.findall(pattern, data)
    path = res[0][0]
    num_lines = res[0][1]
    path = path.replace('\\r\\n', '')
    num_lines = num_lines.replace('\\r\\n', '')

    return path, num_lines


def tail(path, num_lines):
    completed_proc = subprocess.run(['tail', path, '-n', num_lines], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if completed_proc.returncode:
        return completed_proc.stderr.decode('utf-8')
    return completed_proc.stdout.decode('utf-8')

def get_log_strings(path, num_lines):
    return tail(path, num_lines)

class S(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        self._set_headers()
        self.wfile.write("<html><body><h3>httpServer is working<br>Use POST method to access to log files</h3></body></html>".encode('utf-8'))

    def do_HEAD(self):
        self._set_headers()

    def do_POST(self):
        # Doesn't do anything with posted data
        request_headers = self.headers
        content_length = int(request_headers['Content-Length'])
        body = self.rfile.read(content_length)

        try:
            path, num_lines = parse_row_data(body)
            resp = get_log_strings(path, num_lines)
        except Exception as e:
            resp = str(e)

        resp = resp.replace('\n', '<br>')

        resp_body = '<html><body>{}</body></html>'.format(resp)

        self._set_headers()
        self.wfile.write(resp_body.encode('utf-8'))


def run(server_class=HTTPServer, handler_class=S, port=80):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print ('Starting httpd... at port ', port)
    httpd.serve_forever()


if __name__ == "__main__":
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()