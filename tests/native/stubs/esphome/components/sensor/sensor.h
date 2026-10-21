#pragma once
#include <cmath>
#include <functional>
#include <vector>
#include "esphome/core/preferences.h"
namespace esphome {
namespace sensor {
class Sensor {
 public:
  float state{NAN};
  uint32_t pref_key{0};
  std::vector<float> publications;
  void publish_state(float value) {
    state = value;
    publications.push_back(value);
    for (auto& callback : callbacks_)
      callback(value);
  }
  void add_on_raw_state_callback(std::function<void(float)> callback) {
    callbacks_.push_back(callback);
  }
  template <class T>
  ESPPreferenceObject make_entity_preference() {
    return ESPPreferenceObject(pref_key);
  }

 private:
  std::vector<std::function<void(float)>> callbacks_;
};
}  // namespace sensor
}  // namespace esphome
