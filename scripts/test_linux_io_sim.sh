#!/usr/bin/env sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT=$("$ROOT_DIR/build_out/linux_io_sim")

printf '%s\n' "$OUT"

printf '%s\n' "$OUT" | grep -q 'linux IO simulator start'
printf '%s\n' "$OUT" | grep -q 'after_10ms channel=door value=0'
printf '%s\n' "$OUT" | grep -q 'event=rising channel=door type=di value=1'
printf '%s\n' "$OUT" | grep -q 'pressure channel=pressure_ma_x100 value=1200'
printf '%s\n' "$OUT" | grep -q 'event=changed channel=relay type=do value=1'
printf '%s\n' "$OUT" | grep -q 'relay_raw=1'
printf '%s\n' "$OUT" | grep -q 'event=falling channel=door type=di value=0'

printf 'linux IO simulator test passed\n'

