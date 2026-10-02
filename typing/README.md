# C Typing Test

Pequeño juego / test de mecanografía en C que se ejecuta en la terminal. Muestra un texto de referencia y mide tu velocidad (palabras por minuto) y precisión mientras lo escribes.

Texto de referencia actual:

> `hola yo me llamo diego raul como te llamas tu`

## Características

- Lectura de teclado en tiempo real (modo no canónico, sin Enter).
- Usa `termios` para desactivar `ICANON` y `ECHO`.
- Pantalla que se redibuja en cada pulsación con `Time`, texto de referencia coloreado y tu entrada.
- Colores ANSI:
  - Fondo verde: caracteres correctos.
  - Fondo rojo: caracteres pendientes.
- Soporte de borrado con Backspace (código 127).
- Al terminar muestra estadísticas:
  - Caracteres correctos.
  - Total de teclas pulsadas.
  - Total de palabras escritas.
  - Tiempo total (s).
  - Velocidad (palabras/min).
  - Precisión (%).

## Requisitos

- Sistema Linux / macOS (usa `termios.h`, `unistd.h` y `sys/time.h`).
- Compilador `gcc`.

## Compilación

```bash
gcc main.c -o main
```

## Uso

```bash
./main
```

1. Al arrancar verás `Time: waiting to start...` y el texto en rojo.
2. Empieza a escribir: el temporizador arranca con la primera tecla.
3. El texto se pone en verde a medida que aciertas.
4. Para terminar:
   - Completa todo el texto correctamente, o
   - Pulsa `Enter`.
5. Al salir verás el resumen, por ejemplo:

```text
Correct 42 chars
Total keys pressed 45
Total words typed 9
Took 12.34 s

Speed 43.76 words/min
Accuracy 93.33 %
```

## Controles

| Tecla     | Acción                          |
| --------- | ------------------------------- |
| Letras    | Añade carácter y comprueba acierto |
| Backspace | Borra último carácter introducido |
| Enter     | Termina el test                 |

## Estructura del código (`main.c`)

- `count_words()`: cuenta palabras separadas por espacios.
- `count_chars()`: longitud del texto (sin `\0`).
- `printfc() / restore_print()`: impresión con colores ANSI.
- `terminal_set_up() / terminal_end()`: activa/restaura modo raw con `termios`.
- `get_elapsed_time_s()`: diferencia en segundos entre dos `struct timeval`.
- `show_screen()`: limpia pantalla (`\033[2J\033[H`), muestra tiempo, texto coloreado según `correctchars` e input del usuario.
- `main()`: bucle con `getchar()`, gestiona `ref_pointer` (progreso correcto) y `usr_pointer` (buffer `user_input[1024]`), calcula WPM como `total_words * 60 / elapsed_time` y precisión como `ref_pointer / total_letters_pressed * 100`.

## Ideas de mejora

- Permitir textos aleatorios o cargados desde fichero.
- Guardar récord / historial.
- Soporte Windows.
- Cálculo de WPM estándar (5 caracteres = 1 palabra).
- Manejo de flechas y Ctrl+C sin dejar la terminal en mal estado.
