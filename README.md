# c-projects

Colección de proyectos pequeños en C, cada uno en su propio directorio.

## Proyectos

| Directorio | Descripción |
| ---------- | ----------- |
| [`http/`](http/) | Servidor HTTP minimalista en C puro, sin dependencias externas. Listener TCP con `fork()` por conexión y respuesta JSON con eco del método, path y versión. Puerto por defecto `8888`. |
| [`typing/`](typing/) | Test de mecanografía en terminal. Muestra un texto de referencia, mide velocidad (palabras/min) y precisión. Usa `termios` (modo no canónico) y colores ANSI. |

Cada proyecto tiene su propio `README.md` con requisitos, compilación y uso:

- [`http/README.md`](http/README.md)
- [`typing/README.md`](typing/README.md)

## Requisitos generales

- `gcc` + libc POSIX
- Linux / macOS (usan `socket`, `fork`, `termios`, etc.)

## Compilación rápida

```bash
# Servidor HTTP
gcc http/main.c http/http.c http/tcp.c http/router.c http/template.c http/lib.c -o http/main -Wall -Wextra

# Typing test
gcc typing/main.c -o typing/main
```

## Estructura

```
.
├── http/     # Servidor HTTP en C
├── typing/   # Typing test en terminal
└── README.md
```
