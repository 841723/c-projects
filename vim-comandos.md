# Chuleta Vim / Neovim — ordenada por uso

> `leader` = `Espacio`. Casi todo empieza con `Espacio`.
> Modos: `Normal` (navegar), `Insert` (escribir, con `i`), `Visual` (seleccionar, con `v`), `Comando` (con `:`).

## 1. Lo de todos los días (80% del tiempo)

| Comando | Qué hace |
|---|---|
| `Espacio + w` | Guardar (tu mapa de `init.lua`) |
| `Espacio + q` | Salir / cerrar ventana |
| `i`, `a`, `o` | Insertar: en cursor, después del cursor, nueva línea debajo |
| `Esc` o `Ctrl + [` | Volver a Normal |
| `u`, `Ctrl + r` | Deshacer / rehacer |
| `y`, `yy`, `p`, `P` | Copiar, copiar línea, pegar debajo / encima |
| `d`, `dd`, `D`, `x` | Borrar selección, línea, hasta fin de línea, carácter |
| `c`, `cc`, `C` | Borrar y entrar en Insert: selección, línea, hasta fin de línea |
| `v`, `V`, `Ctrl + v` | Visual carácter / línea / bloque |
| `:w`, `:q`, `:wq`, `:q!` | Guardar, salir, guardar+salir, salir sin guardar |
| `:%s/viejo/nuevo/g` | Reemplazar en todo el fichero |
| `:s/viejo/nuevo/g` | Reemplazar solo en la línea actual |

## 2. Moverse por el fichero (lo segundo más usado)

| Comando | Qué hace |
|---|---|
| `h j k l` | Izquierda, abajo, arriba, derecha |
| `w`, `b`, `e` | Palabra siguiente, atrás, fin de palabra |
| `0`, `^`, `$` | Inicio línea, primer carácter, fin de línea |
| `gg`, `G`, `50G` | Inicio fichero, fin fichero, ir a línea 50 |
| `:50` | Ir a línea 50 |
| `Ctrl + u`, `Ctrl + d` | Media página arriba / abajo |
| `Ctrl + b`, `Ctrl + f` | Página arriba / abajo |
| `zz`, `zt`, `zb` | Centrar cursor, poner arriba, poner abajo |
| `%` | Saltar entre `{ } ( ) [ ]` |
| `*`, `#` | Buscar palabra bajo cursor: siguiente / anterior |
| `/texto`, `?texto`, `n`, `N` | Buscar hacia abajo / arriba, siguiente / anterior |
| `fX`, `FX`, `;`, `,` | Saltar al carácter `X` en línea, atrás, repetir, invertir |

## 3. Navegar por tu interfaz (Telescope + árbol + buffers)

Tus mapas de `init.lua`:

| Comando | Qué hace |
|---|---|
| `Espacio + e` | Abrir/cerrar árbol NvimTree |
| `Espacio + f f` | Buscar ficheros (Telescope) |
| `Espacio + f g` | Buscar texto en proyecto (live_grep, necesita ripgrep) |
| `Espacio + f b` | Listar buffers abiertos |
| `Espacio + f h` | Ayuda de Vim |
| `Espacio + b` | Ojo: es breakpoint de debug, no buffer |

Dentro de Telescope y NvimTree:

| Comando | Qué hace |
|---|---|
| `↑ ↓ / j k`, `Enter`, `Esc` | Moverse, abrir, cerrar en Telescope |
| `Ctrl + n`, `Ctrl + p` | Siguiente / anterior en Telescope |
| `j k`, `Enter`, `a`, `d`, `r`, `R`, `q` | En NvimTree: moverse, abrir, crear, borrar, renombrar, refrescar, cerrar |
| `:NvimTreeToggle`, `:NvimTreeFindFile` | Alternar árbol, localizar fichero actual en árbol |

Buffers, ventanas y pestañas (nativo Vim):

| Comando | Qué hace |
|---|---|
| `:ls`, `:b1`, `:bnext`, `:bprev`, `:bd` | Listar buffers, ir al 1, siguiente, anterior, cerrar |
| `Ctrl + w + w` | Saltar a la siguiente ventana |
| `Ctrl + w + h/j/k/l` | Ir a ventana izquierda/abajo/arriba/derecha |
| `Ctrl + w + s`, `Ctrl + w + v` | Dividir horizontal / vertical |
| `Ctrl + w + q`, `Ctrl + w + o` | Cerrar ventana, quedarse solo con esta |
| `:split fichero`, `:vsplit fichero` | Abrir fichero en split |
| `:tabnew`, `gt`, `gT`, `:tabclose` | Nueva pestaña, siguiente, anterior, cerrar |
| `Ctrl + o`, `Ctrl + i` | Volver atrás / adelante en saltos |

## 4. Código: LSP, autocompletado y formateo (tu `init.lua`)

| Comando | Qué hace |
|---|---|
| `g d` | Ir a definición |
| `g D` | Ir a declaración |
| `g r` | Ver referencias |
| `g i` | Ir a implementación |
| `K` | Documentación flotante (hover) |
| `Espacio + r n` | Renombrar símbolo |
| `Espacio + c a` | Acciones de código (fix, include, etc.) |
| `Espacio + f` | Formatear buffer (conform: clang-format, black, prettier, stylua, shfmt) |
| `[d`, `]d` | Diagnóstico anterior / siguiente |
| `:lua vim.diagnostic.open_float()` | Ver error actual en flotante |
| `:lua vim.diagnostic.setloclist()` | Lista de errores del fichero |
| `:checkhealth vim.lsp` | Comprobar que el LSP va |
| `:Mason` | Ver servidores y formateadores instalados |
| `:LspInfo` | Qué LSP está activo en este buffer |

Autocompletado `nvim-cmp` (en modo Insert):

| Comando | Qué hace |
|---|---|
| `Ctrl + Espacio` | Forzar autocompletado |
| `Tab`, `Shift + Tab` | Siguiente / anterior sugerencia |
| `Enter` | Confirmar sugerencia |
| `↑ ↓` | Moverse por el menú |

## 5. Debug con DAP (C/C++/Rust con codelldb)

| Comando | Qué hace |
|---|---|
| `F5` | Continuar / lanzar debug (pide ejecutable) |
| `F10` | Step over (pasar por encima) |
| `F11` | Step into (entrar en función) |
| `F12` | Step out (salir de función) |
| `Espacio + b` | Poner/quitar breakpoint |

Flujo típico C: `gcc -g main.c -o main` → `F5` → ruta `./main` → `F10/F11`.

## 6. Edición rápida que ahorra mucho tiempo

| Comando | Qué hace |
|---|---|
| `o`, `O` | Nueva línea debajo / encima e Insert |
| `>>`, `<<`, `==` | Indentar, desindentar, autoindentar línea |
| `>`, `<`, `=` + movimiento | En Visual: indentar, desindentar, formatear selección |
| `J` | Unir línea siguiente a la actual |
| `~`, `guu`, `gUU` | Cambiar mayúsculas, todo minúsculas, todo mayúsculas |
| `.` | Repetir última acción (muy potente) |
| `ciw`, `caw`, `di"`, `yi(` | Cambiar palabra, borrar palabra completa, borrar dentro de `" "`, copiar dentro de `( )` |
| `>ap`, `=ap` | Indentar / formatear párrafo |
| `gg=G` | Reindentar todo el fichero |
| `Ctrl + a`, `Ctrl + x` | Incrementar / decrementar número bajo cursor |

## 7. Selección, copiar/pegar sistema y macros

| Comando | Qué hace |
|---|---|
| `ggVG` | Seleccionar todo |
| `"+y`, `"+p` | Copiar/pegar con portapapeles del sistema (tienes `unnamedplus`, normalmente no hace falta) |
| `qa ... q`, `@a`, `@@` | Grabar macro en `a`, ejecutar, repetir |
| `q:` | Historial de comandos |
| `q/`, `q?` | Historial de búsquedas |
| `:reg` | Ver registros (lo copiado) |

## 8. Comandos `:` útiles y salud del entorno

| Comando | Qué hace |
|---|---|
| `:e fichero`, `:w`, `:saveas nuevo` | Abrir, guardar, guardar como |
| `:!gcc main.c -o main` | Ejecutar comando shell sin salir |
| `:term` | Abrir terminal dentro de Neovim |
| `Ctrl + \ + Ctrl + n` | Salir del modo terminal a Normal |
| `:noh` | Quitar resaltado de búsqueda |
| `:set number!`, `:set wrap!` | Alternar números / ajuste de línea |
| `:checkhealth`, `:checkhealth nvim-treesitter` | Diagnóstico general y de parsers |
| `:TSUpdate`, `:Lazy sync` | Actualizar parsers y plugins |
| `:help palabra` | Ayuda (también `Espacio + f h`) |

## Mini-flujos para memorizar

1. **Abrir y moverse:** `nvim .` → `Espacio+e` árbol o `Espacio+ff` fichero → `Enter`.
2. **Editar:** `i` escribir → `Esc` → `u` si la lías → `Espacio+w`.
3. **Buscar:** `/mi_funcion` → `n/N` → `*` sobre variable → `Espacio+fg` para todo el proyecto.
4. **Código C:** `gd` definición → `K` docs → `Espacio+ca` fix → `Espacio+f` formato → `F5` debug.
