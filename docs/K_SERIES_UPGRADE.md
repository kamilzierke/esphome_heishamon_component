# Panasonic K-series migration guide

This guide covers the `k_7kw` profile for Panasonic Aquarea High Performance K-series equipment, including WH-SDC0309K3E5 / WH-UDZ07KE5. The compatibility baseline is **ESPHome 2026.9.1**, ESP-IDF and ESP32-C3. The component version is **0.1.0-k1**.

## Changes in this update

- Bounded UART parsing, packet validation, partial-frame timeouts and separate standard/extra freshness tracking.
- A dedicated ESP32 RX/TX task; entity publication remains in the main loop. ESP8266 retains synchronous TX.
- Queued field changes with masks for shared bytes, fresh-base reconstruction for curve settings and readback-based state publication.
- Signed setpoint encoding and validation of finite values, whole protocol units, bounds and control context.
- XTOP power sampling and a daily energy integrator that excludes uncertain gaps.
- Pressure unit correction, temperature-difference metadata, invalid binary-state handling and conservative fractional-temperature decoding.

Existing TOP/XTOP/SET keys are retained. The K profile does not support Optional PCB polling; configuring OPT entities with this profile fails component setup. PCB emulation and CZ-TAW1 pass-through have not been hardware-qualified by this update.

## Component configuration

Include both `panasonic_heatpump` and `panasonic_heatpump_energy` in `external_components.components`. Pin external Git sources to a reviewed commit or release when deploying. See the repository's [general configuration example](../example_panasonic_heatpump.yaml) for source configuration and the [K regression configuration](../tests/panasonic_heatpump/test_panasonic_heatpump_k.yaml) for local component loading.

The following is a fragment to merge into an ESPHome device configuration. UART pins, board selection, network settings and credentials are installation-specific and are intentionally omitted.

```yaml
uart:
  id: uart_heatpump
  baud_rate: 9600
  data_bits: 8
  parity: EVEN
  stop_bits: 1
  rx_buffer_size: 512
  # Configure tx_pin and rx_pin for the installed interface.

panasonic_heatpump:
  id: heatpump
  uart_id: uart_heatpump
  model_profile: k_7kw
  update_interval: 5s
  response_timeout: 2s
  data_timeout: 15s
  command_timeout: 15s
  log_uart_msg: false
```

Without explicit values, `data_timeout` and `command_timeout` default to `max(3 * update_interval, 2 * response_timeout)`. These are component timing policies, not manufacturer requirements. `log_uart_msg` enables full packet logging for troubleshooting; normal operation does not require it.

## Power and daily energy migration

For the K profile, use the six XTOP fields below. Legacy power fields TOP15, TOP16, TOP38, TOP39, TOP40 and TOP41 are invalid for this profile. Remove them from dashboards and energy calculations instead of treating their invalid values as zero. TOP66 is also unavailable in this profile.

| Field | Quantity | Unit |
|---|---|---|
| XTOP0 | Heating electrical input | W |
| XTOP1 | Cooling electrical input | W |
| XTOP2 | DHW electrical input | W |
| XTOP3 | Heating thermal output | W |
| XTOP4 | Cooling thermal output | W |
| XTOP5 | DHW thermal output | W |

Each valid extra packet is a new sample, even when its payload is unchanged. Do not swap the existing XTOP assignments: inputs are 0-2 and outputs are 3-5.

Replace daily counters based on `total_daily_energy` with `panasonic_heatpump_energy`. The following fragment illustrates one counter; the same pattern applies to each XTOP power sensor:

```yaml
time:
  - platform: sntp
    id: clock
    timezone: UTC

sensor:
  - platform: panasonic_heatpump
    xtop0:
      id: heating_input_power
      name: Heating Input Power

  - platform: panasonic_heatpump_energy
    id: heating_input_daily
    name: Heating Input Energy Daily
    heatpump_id: heatpump
    power_id: heating_input_power
    time_id: clock
    max_sample_gap: 15s
    initial_energy: 0
    restore: true
```

Set the clock timezone to the calendar used for daily accounting. `power_id` must identify XTOP0-5 belonging to the same `heatpump_id`.

The new platform already publishes **kWh**. Remove any old `multiply: 0.001` filter on the energy entity. It receives raw sample events before presentation filters and uses packet reception timestamps and generation numbers.

Integration uses the previous sample's power over a continuous interval. The first sample, an invalid value, a gap longer than `max_sample_gap`, loss of freshness or a large wall-clock correction starts a new baseline without charging the uncertain interval. A valid zero-power sample remains valid. Intervals crossing midnight are split between dates, with one-second calendar resolution. Accounting requires a valid clock; daylight-saving changes do not change elapsed epoch time.

### Persistence and statistics

The previous `total_daily_energy` storage format is **not imported**. The first start defaults to 0 kWh. An explicitly verified `initial_energy` value may initialize a counter without a compatible saved state. Restored records include a version, date and energy, but not an old power sample. A record from a different day does not increase the new day's total.

ESPHome buffers preference writes. Sudden power loss may therefore lose the latest unsaved increment. Preserving entity names does not guarantee compatible preference storage, unique IDs or uninterrupted Home Assistant statistics. Check entity references and statistics after migration. Obsolete entities and retained MQTT discovery records are not removed by firmware.

## Setpoints and control context

| Setting | K-profile range | Step |
|---|---|---|
| SET15 Max Pump Duty | 64-254 | 1 |
| SET20 DHW Heat Delta | -12 to -2 K | 1 K |
| SET21 Heater Delay Time | 10-60 min | 1 min |
| SET22 Heater Start Delta | -10 to -2 K | 1 K |
| SET23 Heater Stop Delta | -8 to 0 K | 1 K |
| SET5-8 in compensation-curve context | -5 to +5 K | 1 K |

Max Pump Duty is a protocol code, not a percentage. Non-finite values, fractional protocol units and incompatible control contexts are rejected before transmission. The context is checked again before a queued change is sent.

Zone `number` and `climate` defaults represent compensation-curve offsets. In confirmed Direct mode, configure an explicit absolute-temperature range: heating 20-60 degrees Celsius or cooling 5-20 degrees Celsius. Room-sensor contexts 3/4 remain blocked until their setpoint encoding is established. The climate UI does not change its range dynamically. Context-dependent zone request entities still expose Celsius metadata, even when their meaning is a curve offset; that presentation limitation remains unresolved.

`readback_matches` means a fresh response after transmission matches the changed fields. It is not a transaction-specific ACK and does not establish which controller caused the observed state. Missing agreement before the deadline produces `readback_unconfirmed`. There is no automatic retry. One-shot commands are sent once and remain `sent_once_unverified`.

## Pressure and decoder changes

TOP64 is converted from kgf/cm2 to bar using a factor of **0.980665**, once. Historical values must not merely be relabelled as bar without handling the changed scale. TOP66 does not provide a trustworthy low-pressure reading for the K profile.

Invalid binary codes are not published as ON. Fraction codes 1-4 correspond to .00, .25, .50 and .75; unknown codes remain invalid. Negative temperatures with a nonzero fractional component remain invalid until their sign convention is confirmed. SET37/38 limits have not been broadened without model-specific evidence.

## Diagnostics

```yaml
text_sensor:
  - platform: template
    name: Heatpump Protocol Diagnostics
    update_interval: 10s
    lambda: return id(heatpump).get_diagnostics();
  - platform: template
    name: Heatpump Raw Model Signature
    update_interval: 60s
    lambda: return id(heatpump).get_model_signature();
```

| Key | Meaning |
|---|---|
| `s`, `e` | Age in seconds and validity of standard/extra data |
| `tx` | Physical transmissions |
| `rx` | Standard and extra response counts |
| `h`, `c` | Header and checksum errors |
| `at`, `rt` | Partial-frame and response timeouts |
| `ov` | Queue overflows |
| `qm` | Maximum RX and command queue depths |
| `lm` | Maximum measured main-loop duration in milliseconds |
| `q` | Pending commands |
| `command` | Latest command status |

An age of `4294967295` means no packet has been received yet. Loop timing is millisecond-resolution telemetry, not a worst-case execution-time or UART task stack measurement. Switch, number, select and climate entities may retain their last reading after freshness expires; consult diagnostics. Sensor and binary_sensor states are invalidated.

Skipped energy time is available per counter through `get_skipped_seconds()`:

```yaml
sensor:
  - platform: template
    name: Heating Energy Skipped Time
    unit_of_measurement: s
    accuracy_decimals: 0
    update_interval: 30s
    lambda: return id(heating_input_daily).get_skipped_seconds();
```

The skipped-time value updates when another valid sample arrives; it does not grow speculatively during an ongoing silence.

## Home Assistant heating-curve card

The [Plotly heating-curve example](examples/heating-curve-card.yaml) provides the base curve, shifted curve, reported zone water target, operating context and native offset controls. See [dashboard setup](HEATING_CURVE_DASHBOARD.md) for entity mapping and required HACS installation.

## Validation and remaining limits

The [native tests](../tests/native/test_native.py) compile the actual C++ parser, scheduler and energy adapter with deterministic UART, clock and preference stubs. Packet fixtures are synthetic, not installation logs. Tests cover checksum/length validation, fragmentation, resynchronization, repeated samples, signed encoding, shared-byte masks, curve updates, readback timeout, context changes, energy gaps, monotonic-clock rollover, day/year boundaries and preference restoration.

The reference hardware assessment used ESP32-C3/ESP-IDF and a K-series unit. It confirmed standard/extra polling, valid telemetry, setpoint readback, heating/DHW transitions and counter restoration after a controlled restart. Physical midnight rollover, a communication outage without restart and abrupt power removal were **not exercised**; their coverage is software-based. Power readings have not been calibrated against an external reference meter.

CI uses ESPHome 2026.9.1 and builds ESP8266, ESP32-S2, ESP32-C3, full and K configurations. This does not imply compatibility with every later ESPHome release or physical qualification of every board. Remaining limits include unknown negative fractional temperatures, unconfirmed SET37/38 bounds, unavailable TOP66, room-sensor setpoint semantics and unqualified PCB/proxy behavior.
