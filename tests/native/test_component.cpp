#include <cassert>
#include <deque>
#include <iostream>
#include "../../components/panasonic_heatpump/panasonic_heatpump.h"
namespace esphome {
uint32_t test_now = 1000;
}
using namespace esphome;
using namespace esphome::panasonic_heatpump;
class Bus : public uart::UARTComponent {
 public:
  std::deque<uint8_t> rx;
  std::vector<std::vector<uint8_t>> sent;
  bool fail_flush{false};
  void write_array(const uint8_t* data, size_t size) override {
    sent.emplace_back(data, data + size);
  }
  bool read_byte(uint8_t* byte) override {
    if (rx.empty())
      return false;
    *byte = rx.front();
    rx.pop_front();
    return true;
  }
  size_t available() override {
    return rx.size();
  }
  uart::UARTFlushResult flush() override {
    return fail_flush ? uart::UARTFlushResult::UART_FLUSH_RESULT_TIMEOUT
                      : uart::UARTFlushResult::UART_FLUSH_RESULT_SUCCESS;
  }
  void reply(std::vector<uint8_t> data) {
    data.back() = protocol::checksum(data.data(), data.size() - 1);
    for (auto byte : data)
      rx.push_back(byte);
  }
};
class HP : public PanasonicHeatpumpComponent {
 public:
  size_t queued() const {
    return commands_.size();
  }
  bool active() const {
    return command_active_;
  }
};
class Observer : public PanasonicHeatpumpEntity {
 public:
  unsigned publications{0}, invalidations{0};
  void publish_new_state(const std::vector<uint8_t>&) override {
    ++publications;
  }
  void invalidate() override {
    ++invalidations;
  }
};
void step(HP& hp, uint32_t ms = 10) {
  test_now += ms;
  hp.loop();
}
std::vector<uint8_t> state() {
  std::vector<uint8_t> data(203, 0x55);
  data[0] = 0x71;
  data[1] = 0xC8;
  data[2] = 1;
  data[3] = 0x10;
  data[22] = 0x11;
  data[28] = 0x55;
  data[199] = 3;
  return data;
}
int main() {
  Bus bus;
  HP hp;
  Observer observed;
  hp.set_uart_parent(&bus);
  hp.set_model_profile(true);
  hp.add_sensor(&observed);
  hp.setup();
  assert(hp.getResponseByte(6) == -1);
  hp.loop();
  step(hp);
  assert(bus.sent.size() == 1 && bus.sent.back()[0] == 0x71);
  bus.reply(state());
  step(hp);
  assert(observed.publications == 1 && hp.standard_fresh());
  assert(hp.getResponseByte(-1) == -1 && hp.getResponseByte(203) == -1);
  assert(!hp.validate_setpoint(42, 1000, 0, 2000));
  assert(!hp.validate_setpoint(42, -128, -200, 200));
  assert(!hp.validate_setpoint(45, 255, 0, 1000));
  // Two new equal payloads must both be processed.
  bus.reply(state());
  step(hp);
  assert(observed.publications == 2);
  const size_t before = hp.queued();
  assert(!hp.queue_fields({{6, 0x12, 0x3F}, {38, 0xFF, 0xFF}}));
  assert(hp.queued() == before);  // invalid compound write is atomic
  assert(hp.set_command_field(0x10, 7, 0x38));
  assert(hp.set_command_field(3, 7, 7));
  assert(hp.queued() == 1);
  step(hp, 300);
  step(hp);
  assert(bus.sent.back()[0] == 0xF1 && bus.sent.back()[7] == 0x13);
  assert(hp.active());
  auto actual = state();
  actual[7] = 0x53;
  bus.reply(actual);
  step(hp);
  assert(!hp.active() && hp.get_diagnostics().find("readback_matches") != std::string::npos);
  // No readback: expire honestly, never report a command as accepted by the pump.
  assert(hp.set_command_byte(123, 105));
  step(hp, 300);
  step(hp);
  assert(hp.active());
  step(hp, 16000);
  assert(!hp.active() && hp.get_diagnostics().find("unconfirmed") != std::string::npos);
  assert(observed.invalidations > 0);
  // Lost context requires a fresh poll, not unguarded access to a stale vector.
  assert(!hp.validate_setpoint(38, 0, -5, 5));
  // A setpoint changes semantic while queued: 5 may be numerically valid in both
  // compensation and cooling-direct, but must never cross contexts unnoticed.
  Bus bus2;
  HP hp2;
  hp2.set_uart_parent(&bus2);
  hp2.set_model_profile(true);
  hp2.setup();
  step(hp2);
  step(hp2);
  bus2.reply(state());
  step(hp2);
  assert(hp2.set_command_byte(133, 39));
  auto changed = state();
  changed[28] = (changed[28] & ~0x0C) | 8;
  bus2.reply(changed);
  step(hp2);
  step(hp2, 300);
  assert(hp2.get_diagnostics().find("context_changed") != std::string::npos);
  for (auto& tx : bus2.sent)
    assert(tx[0] != 0xF1);
  // Physical TX failure has an explicit result, not optimistic readback.
  Bus bus3;
  HP hp3;
  hp3.set_uart_parent(&bus3);
  hp3.setup();
  bus3.fail_flush = true;
  step(hp3);
  step(hp3);
  assert(hp3.get_diagnostics().find("tx_timeout_or_error") != std::string::npos);
  // A complete old reply already in RX before physical TX is not readback.
  Bus bus4;
  HP hp4;
  hp4.set_uart_parent(&bus4);
  hp4.set_model_profile(true);
  hp4.setup();
  step(hp4);
  step(hp4);
  bus4.reply(state());
  step(hp4);
  assert(hp4.set_command_byte(123, 105));
  step(hp4, 300);
  auto old_matching = state();
  old_matching[105] = 123;
  bus4.reply(old_matching);
  step(hp4);
  assert(hp4.active() && hp4.get_diagnostics().find("waiting_readback") != std::string::npos);
  test_now += 10;
  bus4.reply(old_matching);
  step(hp4);
  assert(!hp4.active() && hp4.get_diagnostics().find("readback_matches") != std::string::npos);
  std::cout << "Component scheduler/readback regression passed\n";
}
