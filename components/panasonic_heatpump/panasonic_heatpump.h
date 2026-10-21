#pragma once
#include <atomic>
#include <string>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/components/uart/uart.h"
#include "helpers.h"
#include "decode.h"
#include "commands.h"
#include "reliable_protocol.h"
#ifdef USE_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#endif

#define PANASONIC_HEATPUMP_VERSION "0.1.0-k1"
namespace esphome {
namespace panasonic_heatpump {
#ifdef USE_ESP32
using TransportCounter = std::atomic<uint32_t>;
#else
// ESP8266 uses a single synchronous owner; its toolchain lacks atomic RMW.
struct TransportCounter {
  uint32_t value;
  uint32_t load() const {
    return value;
  }
  void store(uint32_t next) {
    value = next;
  }
};
#endif
enum class ResponseType : uint8_t { UNKNOWN, STANDARD, EXTRA, OPTIONAL };
class PanasonicHeatpumpEntity {
 public:
  virtual void set_id(int id) {
    id_ = id;
  }
  virtual void publish_new_state(const std::vector<uint8_t>& data) = 0;
  virtual void invalidate() {
  }

 protected:
  int id_{-1};
};

class PanasonicHeatpumpComponent : public PollingComponent, public uart::UARTDevice {
 public:
  float get_setup_priority() const override {
    return setup_priority::DATA;
  }
  void dump_config() override;
  void setup() override;
  void update() override;
  void loop() override;
  void set_uart_client(uart::UARTComponent* uart) {
    uart_client_ = uart;
  }
  void set_uart_client_timeout(uint32_t ms) {
    uart_client_timeout_ = ms;
  }
  void set_log_uart_msg(bool value) {
    log_uart_msg_ = value;
  }
  void set_optional_pcb_enabled(bool value) {
    optional_pcb_enabled_ = value;
  }
  void set_model_profile(bool k) {
    k_profile_ = k;
  }
  bool is_k_profile() const {
    return k_profile_;
  }
  void set_response_timeout(uint32_t ms) {
    response_timeout_ = ms;
  }
  void set_data_timeout(uint32_t ms) {
    data_timeout_ = ms;
  }
  void set_command_timeout(uint32_t ms) {
    command_timeout_ = ms;
  }
  bool standard_fresh() const;
  bool extra_fresh() const;
  uint32_t get_last_extra_time() const {
    return last_extra_time_;
  }
  uint32_t get_extra_generation() const {
    return extra_generation_;
  }
  int get_response_byte(int index) const;
  int get_extra_response_byte(int index) const;
  int getResponseByte(int index) const {
    return get_response_byte(index);
  }
  int getExtraResponseByte(int index) const {
    return get_extra_response_byte(index);
  }
  bool get_uart_client_timeout_exceeded() const {
    return uart_client_timeout_exceeded_;
  }
  bool validate_setpoint(uint8_t index, float value, float minimum, float maximum, float step = 1) const;
  bool queue_fields(const std::vector<protocol::Field>& fields);
  bool set_command_byte(uint8_t value, uint8_t index);
  bool set_command_field(uint8_t value, uint8_t index, uint8_t mask, bool impulse = false, bool needs_base = false);
  bool set_command_high_nibble(uint8_t value, uint8_t index) {
    return set_command_field(value << 4, index, 0xF0, false, true);
  }
  bool set_command_low_nibble(uint8_t value, uint8_t index) {
    return set_command_field(value, index, 0x0F, false, true);
  }
  bool set_command_curve(uint8_t value, uint8_t index);
  std::string get_diagnostics() const;
  std::string get_model_signature() const;
  void add_sensor(PanasonicHeatpumpEntity* p) {
    sensors_.push_back(p);
  }
  void add_extra_sensor(PanasonicHeatpumpEntity* p) {
    extra_sensors_.push_back(p);
  }
  void add_binary_sensor(PanasonicHeatpumpEntity* p) {
    binary_sensors_.push_back(p);
  }
  void add_number(PanasonicHeatpumpEntity* p) {
    numbers_.push_back(p);
  }
  void add_select(PanasonicHeatpumpEntity* p) {
    selects_.push_back(p);
  }
  void add_switch(PanasonicHeatpumpEntity* p) {
    switches_.push_back(p);
  }
  void add_climate(PanasonicHeatpumpEntity* p) {
    climates_.push_back(p);
  }
  void add_text_sensor(PanasonicHeatpumpEntity* p) {
    text_sensors_.push_back(p);
  }
  void add_optional_binary_sensor(PanasonicHeatpumpEntity* p) {
    optional_binary_sensors_.push_back(p);
  }
  void add_optional_text_sensor(PanasonicHeatpumpEntity* p) {
    optional_text_sensors_.push_back(p);
  }

 protected:
  enum class EventKind : uint8_t { RX, TX_DONE, CLIENT_REQUEST, STATS };
  struct Event {
    EventKind kind{EventKind::RX};
    protocol::Frame frame{};
    uint32_t bad_header{0}, bad_checksum{0}, timeouts{0};
  };
  void io_step_();
  void post_event_(const Event& event);
  bool dispatch_(const protocol::Frame& frame, uint8_t type);
  void process_event_(const Event& event);
  void process_frame_(const protocol::Frame& frame);
  void schedule_(uint32_t now);
  void expire_data_();
  void publish_standard_();
#ifdef USE_ESP32
  static void transport_task_(void* context);
  QueueHandle_t tx_queue_{nullptr}, event_queue_{nullptr};
  TaskHandle_t transport_task_handle_{nullptr};
#else
  std::deque<protocol::Frame> tx_queue_;
  std::deque<Event> event_queue_;
#endif
  uart::UARTComponent* uart_client_{nullptr};
  protocol::Parser parser_, client_parser_;
  TransportCounter dropped_events_{0}, rx_queue_high_water_{0};
  uint32_t max_loop_ms_{0}, max_command_queue_{0};
  uint32_t observed_drops_{0}, worker_stats_time_{0};
  uint32_t bad_header_{0}, bad_checksum_{0}, parser_timeouts_{0};
  bool log_uart_msg_{false}, optional_pcb_enabled_{false}, k_profile_{false};
  bool uart_client_timeout_exceeded_{false};
  bool standard_valid_{false}, extra_valid_{false}, supports_extra_{false};
  bool poll_due_{true}, extra_due_{false}, optional_due_{false}, control_read_due_{false};
  bool io_busy_{false}, tx_completed_{false}, command_active_{false}, active_sent_{false};
  uint8_t active_request_type_{0};  // 0=none, 0x10=standard, 0x21=extra, 0xF1=command, 0x50=legacy optional
  uint32_t uart_client_timeout_{10000}, last_client_request_time_{0};
  uint32_t response_timeout_{2000}, data_timeout_{15000}, command_timeout_{15000};
  uint32_t last_standard_time_{0}, last_extra_time_{0}, extra_generation_{0};
  uint32_t tx_end_{0}, last_exchange_end_{0}, command_sent_at_{0}, last_control_poll_{0};
  uint32_t tx_count_{0}, standard_count_{0}, extra_count_{0}, response_timeouts_{0};
  protocol::CommandQueue commands_;
  protocol::Changes active_changes_;
  std::string command_status_{"idle"};
  std::vector<uint8_t> heatpump_default_message_, heatpump_extra_message_, heatpump_optional_message_;
  std::vector<PanasonicHeatpumpEntity*> sensors_, extra_sensors_, binary_sensors_, numbers_, selects_, switches_,
      climates_, text_sensors_, optional_binary_sensors_, optional_text_sensors_;
};
}  // namespace panasonic_heatpump
}  // namespace esphome
