# dephy_industrial_io

Reusable industrial IO boundary for Dephy product applications.

## Overview

`dephy_industrial_io` lets product logic use one IO API while the raw signal
source changes underneath: POSIX simulator, Zephyr simulator, GPIO, ADC, or
Modbus-backed registers.

## Key Value

- Tests IO behavior on Linux before hardware is ready.
- Models raw states used by real drivers: normal, fault, stuck-at, and noise.
- Reuses debounce, scaling, event generation, fault state, and bounded payload
  formatting.
- Provides MQTT state/event/command/fault topic helpers.
- Includes Zephyr GPIO, ADC, simulator, and Modbus adapter boundaries.

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

## Architecture Flow

```mermaid
flowchart LR
    Product[Product app] --> API[industrial_io API]
    API --> Core[portable debounce/scale/event core]
    Core --> Driver[dephy_io_driver_t]
    Driver --> Sim[POSIX or Zephyr simulator]
    Driver --> GPIO[Zephyr GPIO]
    Driver --> ADC[Zephyr ADC]
    Driver --> Modbus[Modbus adapter]
    Core --> MQTT[MQTT bridge payloads]
```

## Example User Scenario

```mermaid
flowchart TD
    A[Test author writes IO scenario] --> B[Set raw channel state]
    B --> C[Poll common IO core]
    C --> D[Debounce and scale value]
    D --> E[Emit event or fault]
    E --> F[Publish MQTT payload]
    F --> G[Product test validates field behavior]
```

## Simple Principle

Drivers expose raw signal behavior. The portable core turns raw values into
stable product-facing IO state.

## Docs

- `docs/module_structure.md`: public API, adapter, and test layout.
- `docs/todo.md`: current TODO summary.

## License

MIT. See `LICENSE` and `NOTICE.md`. Reuse and references are allowed, but the
copyright notice and attribution to Judd (judadao) must be preserved.
