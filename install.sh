#!/usr/bin/env bash

set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
NVIM_CONFIG_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/nvim"
NVIM_DATA_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/nvim"

if [[ "${EUID}" -eq 0 ]]; then
    SUDO=()
else
    SUDO=(sudo)
fi

echo "=========================================="
echo "  Neovim: entorno C/C++ y multilenguaje"
echo "=========================================="

if ! command -v apt-get >/dev/null 2>&1; then
    echo "Este instalador requiere Ubuntu o Debian (apt-get)." >&2
    exit 1
fi

echo
echo "[1/5] Actualizando índices e instalando dependencias..."
"${SUDO[@]}" apt-get update
"${SUDO[@]}" apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    cmake \
    pkg-config \
    gdb \
    clang \
    clangd \
    clang-format \
    clang-tidy \
    bear \
    valgrind \
    git \
    curl \
    wget \
    unzip \
    xz-utils \
    ripgrep \
    fd-find \
    xclip \
    nodejs \
    npm \
    python3 \
    python3-pip \
    python3-venv \
    rustc \
    cargo

# Telescope y otras herramientas buscan `fd`; en Debian/Ubuntu el paquete
# fd-find instala el ejecutable como `fdfind`.
if ! command -v fd >/dev/null 2>&1 && command -v fdfind >/dev/null 2>&1; then
    "${SUDO[@]}" ln -sf "$(command -v fdfind)" /usr/local/bin/fd
fi

echo
echo "[2/5] Instalando Neovim estable más reciente..."
case "$(uname -m)" in
    x86_64|amd64)
        NVIM_ARCH="x86_64"
        ;;
    aarch64|arm64)
        NVIM_ARCH="arm64"
        ;;
    *)
        echo "Arquitectura no soportada por el instalador de Neovim: $(uname -m)" >&2
        exit 1
        ;;
esac

NVIM_ARCHIVE="$(mktemp --suffix=.tar.gz)"
trap 'rm -f "$NVIM_ARCHIVE"' EXIT
curl -fL \
    "https://github.com/neovim/neovim/releases/latest/download/nvim-linux-${NVIM_ARCH}.tar.gz" \
    -o "$NVIM_ARCHIVE"
"${SUDO[@]}" rm -rf "/opt/nvim-linux-${NVIM_ARCH}"
"${SUDO[@]}" tar -C /opt -xzf "$NVIM_ARCHIVE"
"${SUDO[@]}" ln -sf "/opt/nvim-linux-${NVIM_ARCH}/bin/nvim" /usr/local/bin/nvim

echo "Neovim instalado: $(nvim --version | awk 'NR == 1 { print $2 }')"

echo
echo "[3/5] Preparando directorios y guardando configuración anterior..."
mkdir -p "$NVIM_CONFIG_DIR" "$NVIM_DATA_DIR/lazy"
if [[ -f "$NVIM_CONFIG_DIR/init.lua" ]] && ! cmp -s "$SCRIPT_DIR/init.lua" "$NVIM_CONFIG_DIR/init.lua"; then
    BACKUP="$NVIM_CONFIG_DIR/init.lua.backup.$(date +%Y%m%d%H%M%S)"
    cp "$NVIM_CONFIG_DIR/init.lua" "$BACKUP"
    echo "Configuración anterior guardada en: $BACKUP"
fi
install -m 0644 "$SCRIPT_DIR/init.lua" "$NVIM_CONFIG_DIR/init.lua"

echo
echo "[4/5] Instalando lazy.nvim..."
LAZY_DIR="$NVIM_DATA_DIR/lazy/lazy.nvim"
if [[ ! -d "$LAZY_DIR/.git" ]]; then
    git clone --filter=blob:none https://github.com/folke/lazy.nvim.git "$LAZY_DIR"
else
    echo "lazy.nvim ya está instalado."
fi

echo
echo "[5/5] Instalando plugins de Neovim..."
nvim --headless "+Lazy! sync" +qa
nvim --headless "+MasonToolsInstallSync" +qa

echo
echo "=========================================="
echo " Instalación terminada"
echo "=========================================="
echo "Neovim: $(nvim --version | awk 'NR == 1 { print $2 }')"
echo "Configuración: $NVIM_CONFIG_DIR/init.lua"
echo
echo "Servidores LSP y formateadores instalados mediante Mason."
echo "Abre nvim y ejecuta :Mason para consultar su estado."
echo "Atajos: <Espacio>ff buscar archivos, <Espacio>fg buscar texto, <Espacio>e árbol, <Espacio>f formatear."
