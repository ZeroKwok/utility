#! python
# -*- coding: utf-8 -*-

import os
import sys
import string
import shutil
import datetime
import subprocess

def RunShell(cmd: str) -> str:
    kwargs = {
        'shell': True,
        'stdout': subprocess.PIPE,
    }
    
    # Python 3.7+ 用 `text`，否则用 `universal_newlines`
    if sys.version_info >= (3, 7):
        kwargs['text'] = True
    else:
        kwargs['universal_newlines'] = True
    
    result = subprocess.run(cmd, **kwargs)
    if result.returncode == 0:
        return result.stdout.strip()
    return None

print(f"git-build-gen: {sys.argv} ...")
if len(sys.argv) < 3:
    print("usage: git-build-gen.py <input_file> <output_file>")
    exit(1)

date   = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
dirty  = RunShell("git diff --quiet HEAD || echo -dirty")
commit = RunShell("git rev-parse --short HEAD")
build  = f"{commit}{dirty} at {date}"

print(f'git-build-gen: build: "{build}", process: "{sys.argv[1]}" -> "{sys.argv[2]}"')

with open(sys.argv[1], 'rb') as src:
    with open(sys.argv[2], 'wb') as dst:
        data = src.read().decode()
        data = string.Template(data).safe_substitute(PROJECT_BUILD_STRING=build)
        dst.write(data.encode())
print(f"git-build-gen: finished!")
