import http.server

class RequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        super().do_GET()
        pass

server = http.server.HTTPServer(('0.0.0.0', 8080), RequestHandler)
server.serve_forever()
