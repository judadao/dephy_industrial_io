# dephy_industrial_io

Reusable industrial IO module for Dephy product applications.

`dephy_industrial_io` gives products one stable IO boundary that works in three
places: Linux tests, Zephyr simulator builds, and real hardware adapters. A
product configures channels and consumes normalized events; the platform driver
is the only part that knows whether the signal came from a simulator, GPIO,
ADC, Modbus, or another field interface.

## Overview

Use this repo when product code needs to read industrial IO signals, simulate
them on Linux, or publish normalized IO state through MQTT. The important
concept is simple: product logic talks to one IO API, while the driver behind it
can be swapped from simulator to real hardware.

## Key Value

- Product logic can be tested before hardware is ready.
- Simulated IO states use the same raw-driver model as real adapters.
- Scaling, debounce, fault handling, and payload formatting are reusable.
- MQTT bridge helpers produce deterministic topics and JSON payloads.
- ESP32 firmware can start with the software simulator, then swap in real IO
  adapters without changing product workflow code.

## How To Use

1. Define channel configs: digital inputs, analog inputs, names, scale, and
   debounce policy.
2. Select a driver: POSIX simulator, Zephyr simulator, GPIO, ADC, or field-bus
   adapter.
3. Initialize the module with `dephy_io_init()`.
4. Poll with `dephy_io_poll()` from the product loop or Zephyr work context.
5. Consume normalized channel state and publish through the MQTT bridge helpers.

Minimal usage:

```c
dephy_io_set_driver(my_driver);
dephy_io_init(channels, channel_count);
dephy_io_poll();
```

## How It Works

All platform behavior enters through `dephy_io_driver_t`. Drivers expose raw
signal state, while the portable core owns debouncing, scaling, event
generation, fault state, and bounded payload formatting. This keeps hardware
code small and lets tests exercise the same behavior that a product will use on
target hardware.

The simulators can model normal signals, faults, stuck-at values, and noise.
Those states are intentionally close to the bottom of the stack so higher-level
product tests do not depend on fake product-only shortcuts.

## Layout

```text
include/dephy_industrial_io/  public API
src/                          portable state, payload, MQTT bridge, Modbus adapter
platform/posix/               Linux raw IO simulator
platform/zephyr/              Zephyr simulator, GPIO, and ADC adapters
examples/                     Linux simulator demo
scripts/                      Linux smoke and simulator benchmark
tests/                        C unit tests
zephyr/                       Zephyr module metadata
```

## Commands

```sh
make -f Makefile.linux
make -f Makefile.linux test
make -f Makefile.linux demo
make -f Makefile.linux bench
scripts/test_zephyr_module.sh --metadata-only
```

`test` validates unit behavior, Linux simulator behavior, simulator benchmark
output, and TODO YAML.

## More Docs

See `docs/module_structure.md` for the full module contract.

## TODO

TODO state is tracked in `docs/todo.yaml` and summarized in `docs/todo.md`.
