import os, re, json, glob

# Parse gl.h for function declarations and categories
header_path = 'tinygl/include/GL/gl.h'
functions = {}
current_cat = 'Misc'
func_re = re.compile(r'\b(gl[A-Za-z0-9_]+)\s*\(')
cat_re = re.compile(r'^\s*/\*\s*(.*?)\s*\*/')
with open(header_path) as f:
    for line in f:
        cat_match = cat_re.match(line)
        if cat_match:
            current_cat = cat_match.group(1)
        m = func_re.search(line)
        if m:
            fname = m.group(1)
            functions[fname] = {'category': current_cat, 'files': []}

# Find function definitions in .c files
c_files = glob.glob('tinygl/src/*.c')
for path in c_files:
    with open(path) as f:
        content = f.read()
    for fname in functions:
        if re.search(r'\b' + re.escape(fname) + r'\b', content):
            functions[fname]['files'].append(os.path.basename(path))

# Save mapping
with open('function_map.json', 'w') as f:
    json.dump(functions, f, indent=2)
