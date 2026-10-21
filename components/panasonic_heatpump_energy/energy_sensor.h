#pragma once
#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/panasonic_heatpump/sensor/panasonic_heatpump_sensor.h"

namespace esphome {
namespace panasonic_heatpump_energy {
// Inspired by ESPHome total_daily_energy (ESPHome contributors).
// See docs/LICENSE-ESPHome.txt for the source project licensing.
// Independent implementation: the upstream class is final and cannot reset its
// time baseline on a discontinuity. Units here are always raw W -> kWh.
class DailyEnergy : public sensor::Sensor, public Component {
 public:
  void set_power(panasonic_heatpump::PanasonicHeatpumpSensor* power) {
    power_ = power;
  }
  void set_heatpump(panasonic_heatpump::PanasonicHeatpumpComponent* hp) {
    heatpump_ = hp;
  }
  void set_time(time::RealTimeClock* clock) {
    clock_ = clock;
  }
  void set_max_gap(uint32_t ms) {
    window_.max_gap = ms;
  }
  void set_initial_energy(float kwh) {
    initial_energy_ = kwh;
  }
  void set_restore(bool restore) {
    restore_ = restore;
  }
  void setup() override;
  void loop() override;
  double get_skipped_seconds() const {
    return window_.skipped_seconds;
  }

 protected:
  struct Saved {
    uint32_t version{1}, date{0};
    double energy{0};
  };
  void sample_(float watts);
  bool initialize_(const ESPTime& now);
  void persist_();
  static uint32_t date_(const ESPTime& now) {
    return uint32_t(now.year) * 10000 + now.month * 100 + now.day_of_month;
  }
  panasonic_heatpump::PanasonicHeatpumpSensor* power_{nullptr};
  panasonic_heatpump::PanasonicHeatpumpComponent* heatpump_{nullptr};
  time::RealTimeClock* clock_{nullptr};
  panasonic_heatpump::protocol::EnergyWindow window_;
  ESPPreferenceObject pref_;
  Saved saved_;
  uint32_t date_value_{0}, last_epoch_{0}, last_sample_ms_{0}, last_loop_{0}, last_generation_{0};
  double today_{0};
  float initial_energy_{0};
  bool restore_{true}, loaded_{false}, initialized_{false}, have_epoch_{false};
};
}  // namespace panasonic_heatpump_energy
}  // namespace esphome
