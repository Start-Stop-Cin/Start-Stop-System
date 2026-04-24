# Tests

This folder contains all test implementations for the Start-Stop System, including unit tests for each module and integration tests.

---

## Structure

```text
tests/
├── integration/
│ └── test_integration_sys_ll.c
├── vendor/
│ └── unity/
├── test_co2.c
├── test_ss_control.c
├── test_ss_fuel.c
├── test_ss_inhibit_safety_conditions.c
├── test_ss_operation.c
├── test_timer.c
```
---

## Unit Tests

Each module has a dedicated test file:

- `test_ss_operation.c`
- `test_ss_fuel.c`
- `test_co2.c`
- `test_timer.c`
- `test_ss_control.c`
- `test_ss_inhibit_safety_conditions.c`

### Characteristics

- Built using the **Unity** framework
- Validate modules in isolation
- Cover:
  - Functional behavior
  - Edge cases
  - Robustness (e.g., null inputs)
- Designed to achieve:
  - High Statement coverage
  - High Branch coverage
  - MC/DC coverage

---

## Integration Tests

Located in:

[tests/integration](./tests/integration)

### Purpose

- Validate interaction between multiple modules
- Simulate system behavior similar to the application layer
- Focus on System Low-Level requirements

### Covered Interactions

- AutoStop activation
- Engine restart conditions
- Safety inhibition logic
- Drive cycle memory behavior
- Interaction with Fuel and CO₂ estimation modules

---

## Running Tests

### Run Unit Tests

```bash
make test
```

### Run Coverage

```bash
make coverage
```

### Run Mutation Testing

```bash
make mutation
```

### Run Integration Tests

```bash
make integration
```

---

## Unity Framework

The Unity framework is included as a submodule:

[tests/vendor/unity/](./tests/vendor/unity)

---

## Notes

- Integration tests do not generate coverage reports
- The ss_operation module uses test wrappers (enabled via UNIT_TEST) to allow validation of internal logic for MC/DC coverage
- All tests are executed automatically in the CI pipeline

---

## Traceability

All test cases are fully documented in the project testing document, available in:

 [docs/tests](../docs/tests)

The Appendix includes a spreadsheet containing:

- Complete list of test cases
- Traceability to requirements
- Detailed test descriptions and expected results