#!/bin/sh
set -eu

root="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
binary="$root/build_out/unit_industrial_io"

if [ ! -x "$binary" ]; then
    make -f Makefile.linux -C "$root" build_out/unit_industrial_io >/dev/null
fi

start=$(date +%s)
"$binary" >/dev/null
end=$(date +%s)

duration_ms=$(( (end - start) * 1000 ))
printf '{"benchmark":"simulator_state","iterations":1,"duration_ms":%d}\n' "$duration_ms"
