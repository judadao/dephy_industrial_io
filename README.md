# dephy_industrial_io

Reusable industrial IO module for Dephy product applications.

## Overview

`dephy_industrial_io` gives products one IO API for Linux simulation, Zephyr
simulation, and real hardware adapters. Products configure channels and consume
normalized state; drivers provide raw signal behavior.

## Key Value

- Test IO-dependent logic before hardware is ready.
- Swap simulator, GPIO, ADC, and Modbus adapters behind one API.
- Reuse scaling, debounce, fault handling, and MQTT payload formatting.
- Simulate realistic states: normal, fault, stuck-at, and noise.

## How To Use

```c
dephy_io_set_driver(my_driver);
dephy_io_init(channels, channel_count);
dephy_io_poll();
```

```sh
make -f Makefile.linux test
make -f Makefile.linux demo
make -f Makefile.linux bench
scripts/test_zephyr_module.sh --metadata-only
```

## Simple Principle

The portable core owns IO state, debounce, scaling, events, and payloads. The
platform driver only reads or writes raw signals.

## Docs

- `docs/module_structure.md`: module contract and adapter boundaries.
- `docs/todo.md`: current TODO summary.
