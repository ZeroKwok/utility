#! python
# -*- coding: utf-8 -*-

import os
import sys
import string
import shutil
import datetime
import subprocess

def RunShell(cmd) -> str:
    result = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, text=True)
    if result.returncode == 0:
        return result.stdout.strip()
    return None

print(f"git-build-gen: {sys.argv} ...")
if len(sys.argv) < 3:
    print("usage: git-build-gen.py <output_dir> <output_file>")
    exit(1)
    
date   = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
dirty  = RunShell("git diff --quiet HEAD || echo -dirty")
commit = RunShell("git rev-parse --short HEAD")
build  = f"{commit}{dirty} at {date}"

print(f'git-build-gen: build: "{build}", process: "{sys.argv[1]}" -> "{sys.argv[2]}"')

with open(sys.argv[1], 'rb') as src:
    with open(sys.argv[2], 'wb') as dst:
        data = src.read().decode()
        data = string.Template(data).safe_substitute(UTILITY_BUILD_STRING=build)
        dst.write(data.encode())
print(f"git-build-gen: finished!")
