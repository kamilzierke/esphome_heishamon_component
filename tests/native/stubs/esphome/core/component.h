#pragma once
#include <cstdint>
namespace esphome {
template <class T>
class Parented {
 public:
  void set_parent(T* value) {
    parent_ = value;
  }

 protected:
  T* parent_{nullptr};
};
extern uint32_t test_now;
inline uint32_t millis() {
  return test_now;
}
namespace setup_priority {
constexpr float DATA = 600;
}
class Component {
 public:
  virtual ~Component() = default;
  virtual void setup() {
  }
  virtual void loop() {
  }
  virtual void dump_config() {
  }
  virtual float get_setup_priority() const {
    return 0;
  }
  bool is_failed() const {
    return failed_;
  }
  void mark_failed() {
    failed_ = true;
  }

 private:
  bool failed_{false};
};
class PollingComponent : public Component {
 public:
  virtual void update() {
  }
  uint32_t get_update_interval() const {
    return 5000;
  }
};
}  // namespace esphome
