#!/usr/bin/env python3

import subprocess
import sys

print(subprocess.check_output([sys.executable, "mbed-cmake/configure_for_target.py", "-a", "mbed_app.json", "-x", ".", "-i", ".mbedignore", "NUCLEO_H743ZI2"]))
