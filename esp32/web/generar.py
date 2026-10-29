import os

try:
    Import("env")
    raiz = env["PROJECT_DIR"]
except NameError:
    raiz = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

src = os.path.join(raiz, "web", "index.html")
dst = os.path.join(raiz, "include", "pagina.h")

with open(src, encoding="utf-8") as f:
    html = f.read()

with open(dst, "w", encoding="utf-8") as f:
    f.write("#pragma once\n// Generado desde web/index.html por web/generar.py\n")
    f.write('static const char PAGINA[] PROGMEM = R"HTML(' + html + ')HTML";\n')
