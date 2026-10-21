# Panasonic heat pump tests

The tests cover the configuration schemas, the actual C++ transport/control/energy code, and ESPHome configuration/build compatibility. CI uses **ESPHome 2026.9.1**, not an unpinned latest release.

## Test groups

- `test_panasonic_heatpump_unit.py`: platform imports, metadata, schema structure and code generation.
- `test_panasonic_heatpump_integration.py`: ESPHome configuration loading for the existing board examples.
- `test_setpoint_ranges.py`: finite values, integer protocol units, range/step checks and encoding bounds.
- `../native/test_native.py`: compiles and runs the real C++ parser, component and energy adapter using deterministic UART, clock and preference stubs.

The native suite includes packet framing/checksum/resynchronization, repeated samples, shared-byte masks, curve updates, readback/control-context behavior, energy gaps, clock rollover, calendar boundaries and persistence. The hex fixtures are **synthetic protocol test vectors**, not equipment logs. The packet decoder assertions use constructed values, including 1000 W heating input and 4000 W heating output.

## Run locally

Install the test dependencies and the same ESPHome version used in CI. A C++17-capable `g++` must be available on PATH for native tests.

```sh
python -m pip install -r tests/requirements.txt esphome==2026.9.1
python -m pytest tests/panasonic_heatpump tests/native -v
```

To run only the native suite:

```sh
python -m pytest tests/native/test_native.py -v
```

To validate the K-series configuration without uploading firmware:

```sh
esphome config tests/panasonic_heatpump/test_panasonic_heatpump_k.yaml
```

These commands run from the repository root. Test Wi-Fi credentials are deliberate build-only placeholders. UTC in the K configuration is a neutral test choice; installations must select their own accounting timezone.

## Configuration and build matrix

| File | Purpose |
|---|---|
| `test_panasonic_heatpump_esp8266.yaml` | ESP8266 configuration/build |
| `test_panasonic_heatpump_esp32s2.yaml` | ESP32-S2 configuration/build |
| `test_panasonic_heatpump_esp32c3.yaml` | ESP32-C3 configuration/build |
| `test_panasonic_heatpump_full.yaml` | Full generic feature configuration/build |
| `test_panasonic_heatpump_k.yaml` | ESP32-C3/ESP-IDF K profile and energy configuration/build |
| `test_panasonic_heatpump_mini.yaml` | Minimal configuration validation |
| `test_panasonic_heatpump_cztaw1.yaml` | Proxy configuration coverage; not hardware qualification |

The [workflow](../../.github/workflows/test_panasonic_heatpump.yml) builds five configurations: ESP8266, ESP32-S2, ESP32-C3, full and K. It also runs Black, flake8, clang-format, Python/native tests, and minimal/full configuration validation. Proxy configuration coverage is not a sixth firmware-build job.

A successful configuration check or build does not establish physical behavior on every board. See the [migration guide](../../docs/K_SERIES_UPGRADE.md#validation-and-remaining-limits) for the scope of hardware observations and untested cases.
