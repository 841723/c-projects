# Servidor HTTP en C

Servidor HTTP minimalista escrito en C puro, sin dependencias externas. Implementa un listener TCP con `fork()` por conexión y una capa HTTP que parsea la request line y responde en JSON con eco del método, path y versión.

Puerto por defecto: `8888`.

## Estructura

```
.
├── main.c            # Punto de entrada, gestiona SIGINT y arranca http_listen()
├── http.h / http.c   # Capa HTTP: parse, routing básico, build response, handler
├── tcp.h / tcp.c     # Capa TCP genérica: socket, bind, listen, accept + fork
├── router.c / routes.h # Router minimalista (GET + find)
├── http_constants.h  # Constantes HTTP: métodos, status, headers, content-types
├── config.h          # Límites: MAX_LENGTH_REQUEST/RESPONSE (2048)
├── template.h / template.c # Reservado para plantillas HTML
├── lib.h / lib.c     # Utilidades (contar_digitos)
├── send_http.sh      # Prueba con curl
├── send_tcp.sh       # Prueba con netcat
└── main              # Binario compilado
```

### Capas

1. **TCP (`tcp.c`):** no sabe nada de HTTP. Solo acepta conexiones, hace `fork()` por cliente y delega a un `tcp_handler_t(client_fd, userdata)`.
2. **HTTP (`http.c`):** adaptador `http_handler()` compatible con la firma TCP. Lee con `recv()`, parsea con `sscanf()`, genera respuesta JSON y envía con `send()`.
3. **Router (`router.c`):** `router_get()` / `router_find()`. Actualmente `route_request()` en `http.c` aún no lo integra (TODO) y responde siempre `200 OK`.

## Requisitos

- GCC + libc POSIX (`socket`, `fork`, `getaddrinfo`)
- Linux / macOS
- `curl` y `nc` para los scripts de prueba (opcional)

## Compilación

```bash
gcc main.c http.c tcp.c router.c template.c lib.c -o main -Wall -Wextra
```

## Ejecución

```bash
./main
# Server started listening on port 8888...
```

Parar con `Ctrl+C` (captura `SIGINT` y cierra el socket listener).

## Pruebas

En otra terminal, con el servidor corriendo:

```bash
# Petición HTTP completa
./send_http.sh
# o:
curl -v http://localhost:8888/hola

# Petición TCP cruda
./send_tcp.sh
# o:
echo -n "GET /test HTTP/1.1" | nc localhost 8888
```

Respuesta típica:

```http
HTTP/1.1 200 OK
Content-Type: application/json

{"method_used": "GET","path": "/","version": "HTTP/1.1"}
```

## API principal

```c
// http.h
void http_handler(int client_fd, void *userdata);
int http_listen(const char *port, void (*on_listen)(void));
void http_stop_listener(void);

// tcp.h
typedef void (*tcp_handler_t)(int client_fd, void *userdata);
int tcp_listen(const char *port, tcp_handler_t handler, void *userdata, void (*on_listen)(void));
void tcp_stop_listener();
```

## Limitaciones / TODO

- Solo parsea la request line (`METHOD PATH VERSION`), ignora headers y body.
- `route_request()` no usa el router, siempre devuelve eco JSON.
- `router.c` tiene bugs conocidos (`memset` / `strcmp` en `routes_add` / `router_find`).
- Sin `Content-Length`, sin keep-alive, un `fork()` por conexión.
- Buffers fijos de 2048 bytes (`config.h`).

## Licencia

Sin licencia definida. Uso educativo.
