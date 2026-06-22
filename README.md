# dephy_industrial_io

Reusable industrial IO module for Dephy product applications.

The module owns the portable IO boundary: channel config, scaling, debounce,
event generation, bounded payload formatting, MQTT bridge helpers, simulator
drivers, and hardware adapter APIs. Product code should configure channels,
select a driver, and call the public API.

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

See `docs/module_structure.md` for the full contract.

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

## Driver Boundary

All platform behavior enters through `dephy_io_driver_t`:

```c
dephy_io_set_driver(my_driver);
dephy_io_init(channels, channel_count);
dephy_io_poll();
```

The POSIX and Zephyr simulators model raw signal states used by real drivers:
normal, fault, stuck-at, and noise. ESP32 firmware can first run the Zephyr
software simulator, then swap in GPIO/ADC/Modbus adapters.

## MQTT Bridge

`payload.h` and `mqtt_bridge.h` provide deterministic topic and JSON payload
helpers for product and testkit integration.

## TODO

TODO state is tracked in `docs/todo.yaml` and summarized in `docs/todo.md`.
