# ESPHome Panasonic Heatpump (HeishaMon) Component

This repository provides an ESPHome external component for Panasonic heat pumps that speak the HeishaMon UART protocol over the CN-CNT connector. It lets an ESP8266/ESP32-class device read live telemetry and control heat pump modes, zones, and DHW settings through ESPHome.

## Component functionality

With this component you can:

- Read a wide set of heat pump telemetry values (temperatures, power usage/production, pressures, flow, and status flags) as ESPHome sensors and binary sensors.
- Control operating modes, setpoints, zone configuration, and DHW settings through ESPHome numbers, selects, switches, and climates.
- Operate either in polling mode (ESP sends requests) or in UART client pass-through mode (e.g., alongside a Panasonic CZ-TAW1 module).
- Enable raw UART logging for troubleshooting and advanced analysis.
- Build custom entities by decoding specific protocol bytes when you need values not exposed by default.

## Improvements in this repository

Compared to the original component, this repository focuses on making it easier to adopt and validate:

- Adds Optional PCB (OPT0–OPT6) polling and entity support, including mixing-valve state text sensors and optional pump/alarm binary sensors.
- Documents and ships a full example configuration that includes all readable TOP/XTOP/OPT entities plus daily energy sensors for XTOP power metrics.
- Includes a ready-to-use example configuration (`example_panasonic_heatpump.yaml`) for fast onboarding.
- Provides detailed, comprehensive configuration documentation with full entity lists in `components/panasonic_heatpump/README.md`.
- Adds wiring guidance and a prototype diagram in `prototypes/panasonic_heatpump/` to simplify hardware setup.
- Supplies test configurations under `tests/` for quick validation of builds and future changes.

## Credits

Original creator: Elvit — https://github.com/ElVit/esphome_components/

Thanks to the HeishaMon project and the broader ESPHome/Home Assistant communities for the protocol research and collaboration.
