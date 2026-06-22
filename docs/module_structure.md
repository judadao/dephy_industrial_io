# dephy_industrial_io Module Structure

`dephy_industrial_io` provides the reusable industrial IO boundary for product
applications. Products configure channels and select a driver; this module owns
debounce, scaling, event generation, payload formatting, MQTT bridge helpers,
simulator behavior, and hardware adapter APIs.

## Public Headers

- `industrial_io.h`: channel model, driver interface, polling, reads, writes,
  and event callback registration.
- `payload.h`: topic and JSON payload formatting helpers.
- `mqtt_bridge.h`: MQTT state/event/command bridge helpers.
- `posix_sim.h`: Linux simulator control API.
- `zephyr_sim.h`: Zephyr software simulator control API.
- `zephyr_gpio.h`: Zephyr GPIO adapter configuration.
- `zephyr_adc.h`: Zephyr ADC adapter and industrial scaling helpers.
- `modbus_adapter.h`: Modbus point mapping through caller-provided transport
  callbacks.

## Source Layout

- `src/industrial_io.c`: portable channel state machine.
- `src/payload.c`: bounded topic and JSON formatting.
- `src/mqtt_bridge.c`: reusable MQTT topic and command integration.
- `src/modbus_adapter.c`: Modbus-backed IO driver adapter.
- `platform/posix/io_posix_sim.c`: Linux raw IO simulator.
- `platform/zephyr/io_zephyr_sim.c`: Zephyr software simulator.
- `platform/zephyr/io_zephyr_gpio.c`: GPIO-backed DI/DO adapter.
- `platform/zephyr/io_zephyr_adc.c`: ADC-backed AI adapter.

## Validation

- `tests/unit_industrial_io.c` covers the portable state machine, simulator,
  payload helpers, MQTT bridge, Zephyr-compatible simulator behavior, ADC
  scaling helpers, and Modbus adapter behavior.
- `scripts/test_linux_io_sim.sh` exercises the Linux simulator demo path.
- `scripts/test_zephyr_module.sh` validates Zephyr module metadata.

## Integration Rule

Products should call `dephy_io_set_driver()`, `dephy_io_init()`, and
`dephy_io_poll()` through the public API. If a product needs a new IO behavior,
add it here first, test it here, tag this module, then bump the product
dependency pin.
