# TODO

## Simulator Parity

- [x] Keep POSIX and Zephyr simulator drivers behavior-compatible.
- [x] Model raw signal states used by real drivers: normal, fault, stuck-at, and noise.
- [x] Add AO coverage to simulator unit tests.
- [ ] Add formal event payload helper shared by product and testkit.

## Hardware Drivers

- Add board-specific Zephyr GPIO mapping using devicetree specs.
- Add ADC adapter and calibration helpers for 0-10V and 4-20mA inputs.
- Add MQTT bridge helper for state, event, command, and fault topics.
- Add Modbus RTU/TCP adapter as a separate integration layer.
