# #!/usr/bin/env python3
# import os

# method = os.environ.get('REQUEST_METHOD')  # "GET"

# print("Content-Te: text/html")
# print()  # Blank line separates headers from body
# print("<html>")
# print("<body>")
# print("<h1>Hello World from CGI!</h1>")
# print(f"<p>Method: {method}</p>")
# print("</body>")
# print("</html>")

#!/usr/bin/env python3
import time


time.sleep(10)

print("Content-Type: text/html\r\n")
print("\r\n\r\n")
print("<html><body>Too late</body></html>")
