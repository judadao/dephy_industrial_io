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

## repo

- [x] Align repository layout with dephy_module_golden_sample.
- [x] Add AGENTS.md with module workflow, build, and testing guidance.
- [x] Add docs/module_structure.md describing public API, source, Zephyr, simulator, and test layout.

## performance

- [ ] Add a repeatable simulator benchmark for channel count, noise injection, and MQTT payload formatting.

## validation

- [ ] Add tests that fault-inject GPIO, ADC, and Modbus driver errors through the common IO boundary.
