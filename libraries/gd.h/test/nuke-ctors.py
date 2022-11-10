import re
from pathlib import Path

with open('../include/gd.h', 'r') as file:
    files = re.findall(r'#include "(.+?)"', file.read())

for path in files:
    path = Path('..') / path
    if path.exists():
        with open(path, 'r') as file:
            contents = file.read()

        match = re.search(r'class ([a-zA-Z_]+) .+ {[\s\S]+(\1\(\)\s*{[\s\S]+?})', contents)
        if match:
            with open(path, 'w') as file:
                file.write(contents.replace(match.group(2), ''))