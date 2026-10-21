#pragma once
// Pure protocol primitives, shared by the device and host regression tests.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>
namespace esphome {
namespace panasonic_heatpump {
namespace protocol {
constexpr size_t MAX_FRAME = 203;
constexpr size_t COMMAND_SIZE = 111;
inline bool elapsed(uint32_t now, uint32_t since, uint32_t duration) {
  return uint32_t(now - since) >= duration;
}
inline bool valid_number(float value, float minimum, float maximum, float step = 1) {
  if (!std::isfinite(value) || !std::isfinite(minimum) || !std::isfinite(maximum) || !std::isfinite(step) ||
      minimum > maximum || step <= 0 || value < minimum || value > maximum)
    return false;
  const float grid = (value - minimum) / step;
  return std::abs(grid - std::round(grid)) < 0.0001f && std::abs(value - std::round(value)) < 0.0001f;
}
inline uint8_t checksum(const uint8_t* data, size_t size) {
  uint8_t sum = 0;
  for (size_t i = 0; i < size; ++i)
    sum += data[i];
  return uint8_t(0 - sum);
}
struct Frame {
  std::array<uint8_t, MAX_FRAME> data{};
  uint16_t size{0};
  uint32_t started_at{0}, received_at{0};
};
class Parser {
 public:
  bool allow_optional{false};  // compatibility only, not PCB emulation
  bool requests{false};
  uint32_t bad_header{0}, bad_checksum{0}, timeouts{0};
  void tick(uint32_t now) {
    if (used_ && (elapsed(now, started_, 1000) || elapsed(now, last_, 150))) {
      ++timeouts;
      used_ = 0;
    }
  }
  template <typename Callback>
  void feed(uint8_t byte, uint32_t now, Callback&& callback) {
    tick(now);
    if (!used_ && byte != 0x71 && byte != 0x31 && !(requests && byte == 0xF1))
      return;
    if (!used_)
      started_ = now;
    if (used_ == MAX_FRAME) {
      ++bad_header;
      discard_first_();
    }
    byte_times_[used_] = now;
    buffer_[used_++] = byte;
    last_ = now;
    while (used_) {
      const size_t expected = used_ >= 2 ? size_t(buffer_[1]) + 3 : 0;
      bool ok = buffer_[0] == 0x71 || buffer_[0] == 0x31 || (requests && buffer_[0] == 0xF1);
      if (used_ >= 2)
        ok = ok &&
             ((expected == 203 && buffer_[0] == 0x71) || (buffer_[0] == 0x31 && (expected == 8 || expected == 51)) ||
              (requests && expected == COMMAND_SIZE) || (allow_optional && expected == 20));
      if (used_ >= 3)
        ok = ok && (buffer_[2] == 1 || (buffer_[0] == 0x31 && buffer_[2] == 0x10));
      if (used_ >= 4) {
        const bool normal = buffer_[3] == 0x10 || buffer_[3] == 0x21;
        ok = ok && ((buffer_[0] == 0x31 && (buffer_[3] == 1 || buffer_[3] == 0x10)) ||
                    ((expected == 203 || (requests && expected == COMMAND_SIZE)) && normal) ||
                    (allow_optional && expected == 20 && buffer_[3] == 0x50));
      }
      if (!ok) {
        ++bad_header;
        discard_first_();
        continue;
      }
      if (expected && used_ >= expected) {
        // Preserve the old fork's optional checksum exception only when opted in.
        if (!(allow_optional && expected == 20 && buffer_[3] == 0x50) && checksum(buffer_.data(), expected) != 0) {
          ++bad_checksum;
          discard_first_();
          continue;
        }
        Frame frame;
        frame.size = expected;
        frame.started_at = byte_times_[0];
        frame.received_at = byte_times_[expected - 1];
        std::copy_n(buffer_.begin(), expected, frame.data.begin());
        discard_(expected);
        callback(frame);
        continue;
      }
      break;
    }
  }

 private:
  void discard_first_() {
    discard_(1);
  }
  void discard_(size_t count) {
    std::move(buffer_.begin() + count, buffer_.begin() + used_, buffer_.begin());
    std::move(byte_times_.begin() + count, byte_times_.begin() + used_, byte_times_.begin());
    used_ -= count;
    if (used_)
      started_ = byte_times_[0];
  }
  std::array<uint8_t, MAX_FRAME> buffer_{};
  std::array<uint32_t, MAX_FRAME> byte_times_{};
  size_t used_{0};
  uint32_t started_{0}, last_{0};
};
struct Field {
  uint8_t index{0}, value{0}, mask{0xFF};
  bool curve{false}, impulse{false}, needs_base{false};
  uint8_t context_mode{0}, context_sensor{0};
};
struct Changes {
  std::array<uint8_t, COMMAND_SIZE> values{}, masks{};
  std::array<uint8_t, 4> context_mode{}, context_sensor{};
  bool curve1{false}, curve2{false}, impulse{false}, needs_base{false};
  uint32_t created_at{0};
  void add(const Field& field) {
    values[field.index] = (values[field.index] & ~field.mask) | (field.value & field.mask);
    masks[field.index] |= field.mask;
    if (field.index >= 38 && field.index <= 41) {
      context_mode[field.index - 38] = field.context_mode;
      context_sensor[field.index - 38] = field.context_sensor;
    }
    if (field.curve) {
      if ((field.index >= 75 && field.index <= 78) || (field.index >= 86 && field.index <= 89))
        curve1 = true;
      else
        curve2 = true;
    }
    impulse |= field.impulse;
    needs_base |= field.needs_base || field.curve || (field.index >= 38 && field.index <= 41);
  }
  bool matches(const std::vector<uint8_t>& data) const {
    if (data.size() != MAX_FRAME || impulse)
      return false;
    for (size_t i = 4; i < COMMAND_SIZE - 1; ++i) {
      uint8_t actual = data[i] & masks[i], desired = values[i] & masks[i];
      if (i == 6 && (masks[i] & 0x0F) == 0x0F && (desired & 15) == 8 && ((actual & 15) == 9 || (actual & 15) == 10))
        actual = (actual & 0xF0) | 8;
      if (i == 5 && (masks[i] & 0x30) == 0x30 && (desired & 0x30) == 0x20 && (actual & 0x30) == 0x30)
        actual = (actual & ~0x30) | 0x20;
      if (actual != desired)
        return false;
    }
    return true;
  }
  bool build(const std::vector<uint8_t>& base, Frame& frame) const {
    if (needs_base && base.size() != MAX_FRAME)
      return false;
    frame = Frame{};
    frame.size = COMMAND_SIZE;
    frame.data[0] = 0xF1;
    frame.data[1] = 0x6C;
    frame.data[2] = 1;
    frame.data[3] = 0x10;
    for (size_t i = 4; i < COMMAND_SIZE - 1; ++i) {
      const bool group1 = (i >= 75 && i <= 78) || (i >= 86 && i <= 89);
      const bool group2 = (i >= 79 && i <= 82) || (i >= 90 && i <= 93);
      if ((curve1 && group1) || (curve2 && group2))
        frame.data[i] = base[i];
      frame.data[i] = (frame.data[i] & ~masks[i]) | (values[i] & masks[i]);
    }
    frame.data[COMMAND_SIZE - 1] = checksum(frame.data.data(), COMMAND_SIZE - 1);
    return true;
  }
};
class CommandQueue {
 public:
  bool enqueue(const std::vector<Field>& fields, uint32_t now) {
    if (fields.empty())
      return false;
    bool impulse = false;
    for (const auto& field : fields) {
      if (field.index < 4 || field.index >= COMMAND_SIZE - 1 || field.mask == 0)
        return false;
      impulse |= field.impulse;
    }
    const bool merge = !impulse && !queue_.empty() && !queue_.back().impulse;
    if (!merge && queue_.size() >= 8)
      return false;
    if (!merge) {
      queue_.emplace_back();
      queue_.back().created_at = now;
    }
    for (const auto& field : fields)
      queue_.back().add(field);
    return true;
  }
  bool empty() const {
    return queue_.empty();
  }
  size_t size() const {
    return queue_.size();
  }
  Changes& front() {
    return queue_.front();
  }
  void pop() {
    queue_.pop_front();
  }

 private:
  std::deque<Changes> queue_;
};
inline float water_temperature(uint8_t integral, uint8_t fractional, uint8_t shift) {
  const int code = (fractional >> shift) & 7, whole = int(integral) - 128;
  if (code < 1 || code > 4 || (whole < 0 && code != 1))
    return NAN;
  return whole + (code - 1) * 0.25f;
}
struct EnergySplit {
  double previous;
  double current;
};
inline EnergySplit split_at_midnight(double kwh, uint32_t duration_ms, uint32_t today_ms) {
  const double fraction = duration_ms && today_ms < duration_ms ? double(today_ms) / duration_ms : 1.0;
  return {kwh * (1.0 - fraction), kwh * fraction};
}
class EnergyWindow {
 public:
  uint32_t max_gap{15000};
  double skipped_seconds{0};
  bool push(float watts, uint32_t now, double& kwh, uint32_t& duration_ms) {
    kwh = 0;
    duration_ms = 0;
    if (!std::isfinite(watts) || watts < 0) {
      invalidate();
      return false;
    }
    const uint32_t duration = now - last_;
    const bool integrate = valid_ && duration <= max_gap;
    if (seen_ && !integrate)
      skipped_seconds += duration / 1000.0;
    if (integrate) {
      duration_ms = duration;
      kwh = double(previous_) * duration / 3600000000.0;
    }
    previous_ = watts;
    last_ = now;
    valid_ = true;
    seen_ = true;
    return integrate;
  }
  void invalidate() {
    valid_ = false;
  }

 private:
  float previous_{0};
  uint32_t last_{0};
  bool valid_{false}, seen_{false};
};
}  // namespace protocol
}  // namespace panasonic_heatpump
}  // namespace esphome
