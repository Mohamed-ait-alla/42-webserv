# HTTP API Reference

## Overview

Webserv implements HTTP/1.0 protocol with support for GET, POST, and DELETE methods. This document describes the server's behavior and how to interact with it.

---

## Base URL
```
http://localhost:8080
```

Replace `8080` with your configured port.

---

## HTTP Methods

### GET - Retrieve Resources

Retrieves files, directory listings, or executes CGI scripts.

**Request:**
```http
GET /path/to/resource HTTP/1.1
Host: localhost:8080
```

**Response:**
```http
HTTP/1.1 200 OK
Server: webserv/1.0
Content-Type: text/html
Content-Length: 1234

...
```

**Examples:**
```bash
# Get homepage
curl http://localhost:8080/

# Get specific file
curl http://localhost:8080/images/photo.jpg

# Execute CGI script
curl http://localhost:8080/cgi-bin/script.py?name=John
```

---

### POST - Submit Data

Submits data to the server (form submissions, file uploads, API requests).

**Request:**
```http
POST /upload.html HTTP/1.1
Host: localhost:8080
Content-Type: application/pdf
Content-Length: 27

------WebKitFormBoundaryv9VZscQHlgNYFQwG
Content-Disposition: form-data; name="filename="; filename="example.pdf"
Content-Type: application/pdf

(binary data here)
------WebKitFormBoundaryv9VZscQHlgNYFQwG--
```

**Response:**
```http
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 45

File Uploaded Successfully ✅
```

**Examples:**
```bash
# Form submission
curl -X POST http://localhost:8080/sign-in.html \
  -d "email=bob@gmail.com&password=secret&rememberme=on"

# File upload
curl -X POST http://localhost:8080/upload.html \
  -F "file=@document.pdf"

# CGI POST
curl -X POST http://localhost:8080/cgi-bin/test_post.py \
  -d "username=admin&password=a1d2m3i4n5"
```

---

### DELETE - Remove Resources

Deletes files from the server.

**Request:**
```http
DELETE /uploads/file.txt HTTP/1.1
Host: localhost:8080
```

**Response:**
```http
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 19

File deleted successfully
```

**Examples:**
```bash
# Delete file
curl -X DELETE http://localhost:8080/uploads/document.pdf
```

---