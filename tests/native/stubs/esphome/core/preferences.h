#pragma once
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>
namespace esphome {
inline std::map<uint32_t, std::vector<uint8_t>> test_preferences;
class ESPPreferenceObject {
 public:
  explicit ESPPreferenceObject(uint32_t key = 0) : key_(key) {
  }
  template <class T>
  bool load(T* out) {
    auto it = test_preferences.find(key_);
    if (it == test_preferences.end() || it->second.size() != sizeof(T))
      return false;
    std::memcpy(out, it->second.data(), sizeof(T));
    return true;
  }
  template <class T>
  bool save(const T* in) {
    auto& bytes = test_preferences[key_];
    bytes.resize(sizeof(T));
    std::memcpy(bytes.data(), in, sizeof(T));
    return true;
  }

 private:
  uint32_t key_;
};
}  // namespace esphome
