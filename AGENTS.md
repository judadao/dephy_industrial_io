# Repository Guidelines

## Project Structure & Module Organization

`include/dephy_industrial_io/` contains the public API. Portable logic lives in
`src/`. Platform adapters live under `platform/posix/` and `platform/zephyr/`.
Zephyr module metadata is under `zephyr/`. Linux examples are in `examples/`,
validation scripts are in `scripts/`, and C unit tests are in `tests/`.

## Build, Test, and Development Commands

- `make -f Makefile.linux` builds Linux artifacts into `build_out/`.
- `make -f Makefile.linux test` runs unit and simulator coverage.
- `make -f Makefile.linux demo` runs the POSIX simulator demo.
- `scripts/test_zephyr_module.sh --metadata-only` checks Zephyr metadata.
- `python3 ../dephy_todo/tools/dephy_todo.py validate docs/todo.yaml` validates
  this repo's TODO source of truth.

## Coding Style & Naming Conventions

Use C11 with four-space indentation and `-Wall -Wextra`. Public symbols use the
`dephy_io_` prefix. Keep portable state-machine behavior in `src/`; adapters
should only translate raw GPIO, ADC, simulator, or Modbus transport behavior to
the common driver interface.

## Module Boundaries

Product applications should include public headers and call the module API.
Reusable IO behavior belongs here, not in product `app/src/`. Board-specific pin
selection belongs in product devicetree or Dephy board-platform configuration;
this module owns the driver interface and adapter implementations.

## Testing Guidelines

Add focused C unit tests for channel state, scaling, debounce, payload
formatting, bridge commands, and adapter error handling. Use simulator tests for
raw IO states such as normal, fault, stuck-at, and noise before adding product
integration coverage.

## TODO Workflow

Use `docs/todo.yaml` as the source of truth and keep `docs/todo.md` generated
from it. Set the relevant TODO to `in_progress` before changing behavior, then
mark it `done` and rerender the summary when the work is complete.
