#!/usr/bin/env sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

if [ "${1:-}" = "--metadata-only" ]; then
    test -f "$ROOT_DIR/zephyr/module.yml"
    test -f "$ROOT_DIR/zephyr/Kconfig"
    test -f "$ROOT_DIR/zephyr/CMakeLists.txt"
    grep -q 'CONFIG_DEPHY_INDUSTRIAL_IO' "$ROOT_DIR/zephyr/CMakeLists.txt"
    printf 'Zephyr module metadata OK\n'
    exit 0
fi

if ! command -v west >/dev/null 2>&1; then
    printf 'error: west not found; use --metadata-only or run inside a Zephyr environment\n' >&2
    exit 1
fi

APP_DIR=$(mktemp -d /tmp/dephy_industrial_io_app.XXXXXX)
trap 'rm -rf "$APP_DIR"' EXIT

cat > "$APP_DIR/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dephy_industrial_io_smoke)
target_sources(app PRIVATE src/main.c)
EOF

mkdir -p "$APP_DIR/src"
cat > "$APP_DIR/src/main.c" <<'EOF'
#include <zephyr/kernel.h>
#include <dephy_industrial_io/industrial_io.h>

int main(void)
{
    return (int)dephy_io_channel_count();
}
EOF

cat > "$APP_DIR/prj.conf" <<'EOF'
CONFIG_DEPHY_INDUSTRIAL_IO=y
EOF

west build -b "${BOARD:-native_sim}" "$APP_DIR" \
    -- -DZEPHYR_EXTRA_MODULES="$ROOT_DIR"

