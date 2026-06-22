# TODO

Source of truth: `docs/todo.yaml`. Update YAML before starting or completing work.

## simulator

- [x] Keep POSIX and Zephyr simulator drivers behavior-compatible.
- [x] Model raw signal states used by real drivers: normal, fault, stuck-at, and noise.
- [x] Add AO coverage to simulator unit tests.

## integration

- [x] Add formal event payload helper shared by product and testkit.
- [x] Add MQTT bridge helper for state, event, command, and fault topics.

## hardware

- [x] Add board-specific Zephyr GPIO mapping using devicetree specs.
- [x] Add ADC adapter and calibration helpers for 0-10V and 4-20mA inputs.
- [x] Add Modbus RTU/TCP adapter as a separate integration layer.
