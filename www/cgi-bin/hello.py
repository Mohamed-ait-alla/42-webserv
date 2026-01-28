#!/usr/bin/env python3

import os
import sys

# CGI header (REQUIRED)
sys.stdout.write("Content-Type: text/plain\r\n\r\n")

sys.stdout.write("CGI Environment Variables:\n")
sys.stdout.write("==========================\n")

for key, value in os.environ.items():
    sys.stdout.write(f"{key}={value}\n")
