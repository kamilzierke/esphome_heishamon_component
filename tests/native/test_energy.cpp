#include <cassert>
#include <cmath>
#include <iostream>
#include "../../components/panasonic_heatpump_energy/energy_sensor.h"
namespace esphome {
uint32_t test_now = 1000;
namespace panasonic_heatpump {
void PanasonicHeatpumpSensor::dump_config() {
}
void PanasonicHeatpumpSensor::publish_new_state(const std::vector<uint8_t>&) {
}
}  // namespace panasonic_heatpump
}  // namespace esphome
using namespace esphome;
using namespace esphome::panasonic_heatpump;
using namespace esphome::panasonic_heatpump_energy;
class HP : public PanasonicHeatpumpComponent {
 public:
  void receive() {
    extra_valid_ = true;
    last_extra_time_ = test_now;
    ++extra_generation_;
  }
};
ESPTime stamp(int year, int month, int day, int hour, int minute, int second) {
  ESPTime result;
  result.year = year;
  result.month = month;
  result.day_of_month = day;
  result.hour = hour;
  result.minute = minute;
  result.second = second;
  result.recalc_timestamp_local();
  return result;
}
int main() {
  HP hp;
  PanasonicHeatpumpSensor power;
  power.set_parent(&hp);
  power.set_id(CONF_XTOP0);
  time::RealTimeClock clock;
  DailyEnergy energy;
  energy.pref_key = 1;
  energy.set_power(&power);
  energy.set_heatpump(&hp);
  energy.set_time(&clock);
  energy.setup();
  hp.receive();
  power.publish_state(1000);
  assert(std::isnan(energy.state));  // no calendar date
  clock.value = stamp(2026, 12, 31, 23, 59, 55);
  hp.receive();
  power.publish_state(1000);
  assert(energy.state == 0);
  test_now += 10000;
  clock.value = stamp(2027, 1, 1, 0, 0, 5);
  hp.receive();
  power.publish_state(1000);
  assert(std::abs(energy.state - 5.0 / 3600) < 1e-8);  // split New Year's midnight
  const float first = energy.state;
  // Replay of a callback from the same frame must not count a second sample.
  power.publish_state(1000);
  assert(energy.state == first);
  test_now += 20000;
  clock.value = ESPTime::from_epoch(clock.value.timestamp + 20);
  hp.receive();
  power.publish_state(1000);
  assert(energy.state == first && energy.get_skipped_seconds() >= 20);
  test_now += 5000;
  clock.value = ESPTime::from_epoch(clock.value.timestamp + 5);
  hp.receive();
  power.publish_state(1000);
  assert(std::abs(energy.state - 10.0 / 3600) < 1e-8);
  power.publish_state(NAN);
  test_now += 5000;
  clock.value = ESPTime::from_epoch(clock.value.timestamp + 5);
  hp.receive();
  power.publish_state(1000);
  assert(std::abs(energy.state - 10.0 / 3600) < 1e-8);
  // A backwards NTP correction resets the baseline.
  test_now += 5000;
  clock.value = ESPTime::from_epoch(clock.value.timestamp - 3600);
  hp.receive();
  power.publish_state(1000);
  assert(energy.state == 0);  // previous date now, no uncertain interval integrated
  test_now += 5000;
  clock.value = ESPTime::from_epoch(clock.value.timestamp + 5);
  hp.receive();
  power.publish_state(1000);
  assert(std::abs(energy.state - 5.0 / 3600) < 1e-8);
  // Restore only energy, never a pre-reboot power baseline.
  DailyEnergy restored;
  restored.pref_key = 1;
  restored.set_power(&power);
  restored.set_heatpump(&hp);
  restored.set_time(&clock);
  restored.setup();
  restored.loop();
  assert(std::abs(restored.state - 5.0 / 3600) < 1e-8);
  clock.value = stamp(2027, 1, 2, 12, 0, 0);
  DailyEnergy another_day;
  another_day.pref_key = 1;
  another_day.set_power(&power);
  another_day.set_heatpump(&hp);
  another_day.set_time(&clock);
  another_day.setup();
  another_day.loop();
  assert(another_day.state == 0);
  PanasonicHeatpumpSensor wrong;
  wrong.set_parent(&hp);
  wrong.set_id(CONF_TOP16);
  DailyEnergy rejected;
  rejected.set_power(&wrong);
  rejected.set_heatpump(&hp);
  rejected.set_time(&clock);
  rejected.setup();
  assert(rejected.is_failed());
  std::cout << "Daily energy adapter regression passed\n";
}
