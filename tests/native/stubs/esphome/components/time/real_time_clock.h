#pragma once
#include <ctime>
#include <cstdint>
namespace esphome {
struct ESPTime {
  uint16_t year{1970};
  uint8_t month{1}, day_of_month{1}, hour{0}, minute{0}, second{0};
  std::time_t timestamp{0};
  bool is_valid() const {
    return year >= 2019;
  }
  static ESPTime from_epoch(std::time_t epoch) {
    const auto value = *std::gmtime(&epoch);
    ESPTime result;
    result.year = value.tm_year + 1900;
    result.month = value.tm_mon + 1;
    result.day_of_month = value.tm_mday;
    result.hour = value.tm_hour;
    result.minute = value.tm_min;
    result.second = value.tm_sec;
    result.timestamp = epoch;
    return result;
  }
  void recalc_timestamp_local() {
    std::tm value{};
    value.tm_year = year - 1900;
    value.tm_mon = month - 1;
    value.tm_mday = day_of_month;
    value.tm_hour = hour;
    value.tm_min = minute;
    value.tm_sec = second;
#ifdef _WIN32
    timestamp = _mkgmtime(&value);
#else
    timestamp = timegm(&value);
#endif
  }
};
namespace time {
class RealTimeClock {
 public:
  ESPTime value;
  ESPTime now() const {
    return value;
  }
};
}  // namespace time
}  // namespace esphome
