# #!/usr/bin/env python3
import os

method = os.environ.get('REQUEST_METHOD')  # "GET"
path = os.environ.get('SCRIPT_NAME')
query = os.environ.get('QUERY_STRING')
host = os.environ.get('SERVER_NAME')
port = os.environ.get('SERVER_PORT')

print("Content-Type: text/html")
print()  # Blank line separates headers from body
print("<html>")
print("<body>")
print("<h1>Hello World from CGI!</h1>")
print(f"<p>Method: {method}</p>")
print(f"<p>scripName: {path}</p>")
print(f"<p>query: {query}</p>")
print(f"<p>Hostname:  {host}</p>")
print(f"<p>Port: {port}</p>")
print("</body>")
print("</html>")

#!/usr/bin/env python3
# import time


# time.sleep(10)

# print("Content-Type: text/html\r\n")
# print("\r\n\r\n")
# print("<html><body>Too late</body></html>")
