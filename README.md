# dephy_industrial_io

Reusable industrial IO module for Dephy product applications.

The module owns portable IO channel state, scaling, debounce, event generation,
and a small driver interface. Platform-specific code supplies raw IO reads and
writes.

## Scope

- Digital input and output channel model.
- Analog input and output channel model.
- Debounce and edge events for digital input.
- POSIX simulator for Linux unit and integration tests.
- Zephyr module metadata for product builds.

## Linux Build And Test

```sh
make -f Makefile.linux
make -f Makefile.linux test
make -f Makefile.linux demo
```

The Linux demo uses the POSIX simulator to fake IO changes:

- `door`: digital input with debounce and rising/falling events.
- `relay`: digital output write path.
- `pressure_ma_x100`: analog input scaling example.

## Zephyr Use

Add this repository to `ZEPHYR_EXTRA_MODULES`, then enable:

```conf
CONFIG_DEPHY_INDUSTRIAL_IO=y
```

Product code should provide channel configuration and select a platform driver:

```c
#include <dephy_industrial_io/industrial_io.h>

static const dephy_io_channel_config_t channels[] = {
    {
        .name = "door",
        .type = DEPHY_IO_DI,
        .driver_channel = 0,
        .debounce_ms = 20,
        .scale_num = 1,
        .scale_den = 1,
    },
};

dephy_io_set_driver(my_driver);
dephy_io_init(channels, 1);
```

## ESP32 IO Simulation

For firmware tests on ESP32 without wiring real IO, enable:

```conf
CONFIG_DEPHY_INDUSTRIAL_IO=y
CONFIG_DEPHY_INDUSTRIAL_IO_ZEPHYR_SIM=y
```

Then use the Zephyr software simulator driver:

```c
#include <dephy_industrial_io/industrial_io.h>
#include <dephy_industrial_io/zephyr_sim.h>

dephy_io_set_driver(dephy_io_zephyr_sim_driver());
dephy_io_init(channels, channel_count);

dephy_io_zephyr_sim_set_raw(0, 1);
dephy_io_poll();
```

This validates product logic, debounce, scaling, MQTT bridge behavior, and UI
workflows. It does not validate electrical behavior. For pin-level testing,
wire an ESP32 output GPIO to an input GPIO and run the GPIO adapter instead.
