#include "panasonic_heatpump.h"
#include <cstdio>
#include <cinttypes>
#include "esphome/core/application.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump";

void PanasonicHeatpumpComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Panasonic Heatpump %s; profile=%s", PANASONIC_HEATPUMP_VERSION, k_profile_ ? "k_7kw" : "generic");
}
void PanasonicHeatpumpComponent::setup() {
  if (k_profile_ && optional_pcb_enabled_) {
    ESP_LOGE(TAG, "k_7kw profile does not enable optional PCB polling");
    mark_failed();
    return;
  }
  parent_->set_flush_timeout(500);
  parser_.allow_optional = optional_pcb_enabled_;
  client_parser_.requests = true;
#ifdef USE_ESP32
  tx_queue_ = xQueueCreate(1, sizeof(protocol::Frame));
  event_queue_ = xQueueCreate(16, sizeof(Event));
  if (!tx_queue_ || !event_queue_ ||
      xTaskCreate(transport_task_, "heatpump_uart", 4096, this, tskIDLE_PRIORITY + 1, &transport_task_handle_) !=
          pdPASS) {
    ESP_LOGE(TAG, "Unable to allocate UART task/queues");
    if (tx_queue_)
      vQueueDelete(tx_queue_);
    if (event_queue_)
      vQueueDelete(event_queue_);
    tx_queue_ = nullptr;
    event_queue_ = nullptr;
    mark_failed();
  }
#endif
}
#ifdef USE_ESP32
void PanasonicHeatpumpComponent::transport_task_(void* context) {
  auto* self = static_cast<PanasonicHeatpumpComponent*>(context);
  for (;;) {
    self->io_step_();
    vTaskDelay(std::max<TickType_t>(1, pdMS_TO_TICKS(5)));
  }
}
#endif
void PanasonicHeatpumpComponent::post_event_(const Event& event) {
#ifdef USE_ESP32
  if (xQueueSend(event_queue_, &event, 0) != pdPASS)
    dropped_events_.store(dropped_events_.load() + 1);
  rx_queue_high_water_.store(std::max(rx_queue_high_water_.load(), uint32_t(uxQueueMessagesWaiting(event_queue_))));
#else
  if (event_queue_.size() < 16)
    event_queue_.push_back(event);
  else
    dropped_events_.store(dropped_events_.load() + 1);
  rx_queue_high_water_.store(std::max(rx_queue_high_water_.load(), uint32_t(event_queue_.size())));
#endif
}
void PanasonicHeatpumpComponent::io_step_() {
  protocol::Frame tx;
#ifdef USE_ESP32
  const bool send = xQueueReceive(tx_queue_, &tx, 0) == pdPASS;
#else
  const bool send = !tx_queue_.empty();
  if (send) {
    tx = tx_queue_.front();
    tx_queue_.pop_front();
  }
#endif
  uint8_t byte;
  // Bound each pass; UART's ring buffer holds bytes until the next pass.
  for (size_t count = 0; count < 256 && available(); ++count) {
    if (!read_byte(&byte))
      break;
    if (uart_client_)
      uart_client_->write_byte(byte);
    parser_.feed(byte, millis(), [this](const protocol::Frame& frame) {
      Event event;
      event.frame = frame;
      event.bad_header = parser_.bad_header;
      event.bad_checksum = parser_.bad_checksum;
      event.timeouts = parser_.timeouts;
      post_event_(event);
    });
  }
  parser_.tick(millis());
  if (protocol::elapsed(millis(), worker_stats_time_, 1000)) {
    worker_stats_time_ = millis();
    Event stats;
    stats.kind = EventKind::STATS;
    stats.bad_header = parser_.bad_header;
    stats.bad_checksum = parser_.bad_checksum;
    stats.timeouts = parser_.timeouts;
    post_event_(stats);
  }
  // Timestamp/drain bytes that were buffered before TX first. They cannot
  // become post-command readback merely because sending took 127 ms.
  if (send) {
    write_array(tx.data.data(), tx.size);
    const auto result = flush();  // only the transport task blocks on ESP32
    Event event;
    event.kind = EventKind::TX_DONE;
    event.frame = tx;
    event.frame.received_at = millis();
    // A failed/late TX must never be presented as confirmed delivery.
    event.frame.started_at = (result == uart::UARTFlushResult::UART_FLUSH_RESULT_SUCCESS ||
                              result == uart::UARTFlushResult::UART_FLUSH_RESULT_ASSUMED_SUCCESS)
                                 ? 1
                                 : 0;
    post_event_(event);
  }
  // Compatibility: the client owns polling while it is active. Its frames are
  // assembled separately and only the transport owner accesses either UART.
  if (uart_client_) {
    for (size_t count = 0; count < 256 && uart_client_->available(); ++count) {
      if (!uart_client_->read_byte(&byte))
        break;
      parent_->write_byte(byte);
      client_parser_.feed(byte, millis(), [this](const protocol::Frame& frame) {
        Event event;
        event.kind = EventKind::CLIENT_REQUEST;
        event.frame = frame;
        post_event_(event);
      });
    }
    client_parser_.tick(millis());
  }
}
void PanasonicHeatpumpComponent::update() {
  poll_due_ = true;
}
bool PanasonicHeatpumpComponent::standard_fresh() const {
  return standard_valid_ && !protocol::elapsed(millis(), last_standard_time_, data_timeout_);
}
bool PanasonicHeatpumpComponent::extra_fresh() const {
  return extra_valid_ && !protocol::elapsed(millis(), last_extra_time_, data_timeout_);
}
int PanasonicHeatpumpComponent::get_response_byte(int index) const {
  if (standard_fresh() && index >= 0 && size_t(index) < heatpump_default_message_.size())
    return heatpump_default_message_[index];
  return -1;
}
int PanasonicHeatpumpComponent::get_extra_response_byte(int index) const {
  if (extra_fresh() && index >= 0 && size_t(index) < heatpump_extra_message_.size())
    return heatpump_extra_message_[index];
  return -1;
}
void PanasonicHeatpumpComponent::loop() {
  if (is_failed())
    return;
  const uint32_t loop_started = millis();
#ifndef USE_ESP32
  io_step_();  // legacy ESP8266 stays supported; TX is synchronous on that platform
#endif
  const uint32_t drops = dropped_events_.load();
  if (drops != observed_drops_) {
    observed_drops_ = drops;
    standard_valid_ = false;
    extra_valid_ = false;
    for (auto* entity : sensors_)
      entity->invalidate();
    for (auto* entity : extra_sensors_)
      entity->invalidate();
    command_status_ = "rx_queue_overflow";
    poll_due_ = true;
    ESP_LOGW(TAG, "UART event queue overflow (%" PRIu32 ")", drops);
  }
  Event event;
  for (unsigned count = 0; count < 4; ++count) {
#ifdef USE_ESP32
    if (xQueueReceive(event_queue_, &event, 0) != pdPASS)
      break;
#else
    if (event_queue_.empty())
      break;
    event = event_queue_.front();
    event_queue_.pop_front();
#endif
    process_event_(event);
  }
  expire_data_();
  schedule_(millis());
  max_loop_ms_ = std::max(max_loop_ms_, uint32_t(millis() - loop_started));
}
void PanasonicHeatpumpComponent::process_event_(const Event& event) {
  if (event.kind == EventKind::CLIENT_REQUEST) {
    last_client_request_time_ = event.frame.received_at;
    uart_client_timeout_exceeded_ = false;
    return;
  }
  if (event.kind == EventKind::TX_DONE) {
    ++tx_count_;
    tx_end_ = event.frame.received_at;
    tx_completed_ = true;
    if (!event.frame.started_at) {
      io_busy_ = false;
      last_exchange_end_ = tx_end_;
      command_status_ = "tx_timeout_or_error";
      if (active_request_type_ == 0xF1)
        command_active_ = false;
      return;
    }
    if (active_request_type_ == 0xF1) {
      active_sent_ = true;
      command_sent_at_ = tx_end_;
      last_control_poll_ = tx_end_;
      command_status_ = active_changes_.impulse ? "sent_once_unverified" : "waiting_readback";
      if (active_changes_.impulse)
        command_active_ = false;
      control_read_due_ = true;
    }
    return;
  }
  bad_header_ = event.bad_header;
  bad_checksum_ = event.bad_checksum;
  parser_timeouts_ = event.timeouts;
  if (event.kind != EventKind::STATS)
    process_frame_(event.frame);
}
void PanasonicHeatpumpComponent::process_frame_(const protocol::Frame& frame) {
  if (log_uart_msg_)
    PanasonicHelpers::log_uart_hex(UART_LOG_RX, frame.data.data(), frame.size, ',');
  const uint8_t type = frame.data[3];
  if (io_busy_ && tx_completed_ && int32_t(frame.started_at - tx_end_) >= 0 &&
      (type == active_request_type_ || (active_request_type_ == 0xF1 && type == 0x10))) {
    io_busy_ = false;
    last_exchange_end_ = frame.received_at;
  }
  if (protocol::elapsed(millis(), frame.received_at, data_timeout_))
    return;
  if (frame.size == 203 && type == 0x10) {
    heatpump_default_message_.assign(frame.data.begin(), frame.data.begin() + frame.size);
    last_standard_time_ = frame.received_at;
    standard_valid_ = true;
    ++standard_count_;
    supports_extra_ = frame.data[199] > 2;
    if (supports_extra_ && !extra_sensors_.empty())
      extra_due_ = true;
    if (optional_pcb_enabled_)
      optional_due_ = true;
    if (command_active_ && active_sent_ && int32_t(frame.started_at - command_sent_at_) >= 0 &&
        active_changes_.matches(heatpump_default_message_)) {
      command_active_ = false;
      control_read_due_ = false;
      command_status_ = "readback_matches";
    }
    publish_standard_();
  } else if (frame.size == 203 && type == 0x21) {
    heatpump_extra_message_.assign(frame.data.begin(), frame.data.begin() + frame.size);
    last_extra_time_ = frame.received_at;
    extra_valid_ = true;
    ++extra_count_;
    ++extra_generation_;
    for (auto* entity : extra_sensors_)
      entity->publish_new_state(heatpump_extra_message_);
  } else if (optional_pcb_enabled_ && frame.size == 20 && type == 0x50) {
    heatpump_optional_message_.assign(frame.data.begin(), frame.data.begin() + frame.size);
    for (auto* entity : optional_binary_sensors_)
      entity->publish_new_state(heatpump_optional_message_);
    for (auto* entity : optional_text_sensors_)
      entity->publish_new_state(heatpump_optional_message_);
  }
}
void PanasonicHeatpumpComponent::publish_standard_() {
  for (const auto* group : {&sensors_, &binary_sensors_, &text_sensors_, &numbers_, &selects_, &switches_, &climates_})
    for (auto* entity : *group)
      entity->publish_new_state(heatpump_default_message_);
}
void PanasonicHeatpumpComponent::expire_data_() {
  if (standard_valid_ && !standard_fresh()) {
    standard_valid_ = false;
    for (const auto* group : {&sensors_, &binary_sensors_, &numbers_, &selects_, &switches_, &climates_})
      for (auto* entity : *group)
        entity->invalidate();
  }
  if (extra_valid_ && !extra_fresh()) {
    extra_valid_ = false;
    for (auto* entity : extra_sensors_)
      entity->invalidate();
  }
}
bool PanasonicHeatpumpComponent::dispatch_(const protocol::Frame& frame, uint8_t type) {
#ifdef USE_ESP32
  if (xQueueSend(tx_queue_, &frame, 0) != pdPASS)
    return false;
#else
  if (!tx_queue_.empty())
    return false;
  tx_queue_.push_back(frame);
#endif
  if (log_uart_msg_)
    PanasonicHelpers::log_uart_hex(UART_LOG_TX, frame.data.data(), frame.size, ',');
  io_busy_ = true;
  tx_completed_ = false;
  active_request_type_ = type;
  // Bound even a lost TX_DONE event; no false update to last physical TX time.
  tx_end_ = millis();
  return true;
}
void PanasonicHeatpumpComponent::schedule_(uint32_t now) {
  if (command_active_ &&
      protocol::elapsed(now, active_sent_ ? command_sent_at_ : active_changes_.created_at, command_timeout_)) {
    command_active_ = false;
    command_status_ = "readback_unconfirmed";
    ESP_LOGW(TAG, "Command expired without matching readback");
  }
  if (io_busy_) {
    if (protocol::elapsed(now, tx_end_, response_timeout_ + (tx_completed_ ? 0 : 1000))) {
      io_busy_ = false;
      last_exchange_end_ = now;
      ++response_timeouts_;
      if (command_active_)
        control_read_due_ = true;
    }
    return;
  }
  if (!protocol::elapsed(now, last_exchange_end_, 250))
    return;
  if (uart_client_) {
    uart_client_timeout_exceeded_ =
        uart_client_timeout_ >= 100 && protocol::elapsed(now, last_client_request_time_, uart_client_timeout_);
    if (!uart_client_timeout_exceeded_)
      return;
  }
  protocol::Frame frame;
  if (command_active_) {
    if (active_sent_ && (control_read_due_ || protocol::elapsed(now, last_control_poll_, get_update_interval()))) {
      std::copy_n(PanasonicCommand::PollingMessage, DATA_MESSAGE_SIZE, frame.data.begin());
      frame.size = DATA_MESSAGE_SIZE;
      if (dispatch_(frame, 0x10)) {
        control_read_due_ = false;
        last_control_poll_ = now;
        poll_due_ = false;
      }
    } else if (extra_due_) {
      std::copy_n(PanasonicCommand::PollingExtraMessage, DATA_MESSAGE_SIZE, frame.data.begin());
      frame.size = DATA_MESSAGE_SIZE;
      if (dispatch_(frame, 0x21))
        extra_due_ = false;
    }
    return;
  }
  // A completed command's control poll precedes the next queued write.
  if (control_read_due_)
    poll_due_ = true;
  if (poll_due_) {
    std::copy_n(PanasonicCommand::PollingMessage, DATA_MESSAGE_SIZE, frame.data.begin());
    frame.size = DATA_MESSAGE_SIZE;
    if (dispatch_(frame, 0x10)) {
      poll_due_ = false;
      control_read_due_ = false;
    }
    return;
  }
  if (extra_due_ && (!extra_fresh() || protocol::elapsed(now, last_extra_time_, data_timeout_ / 2))) {
    std::copy_n(PanasonicCommand::PollingExtraMessage, DATA_MESSAGE_SIZE, frame.data.begin());
    frame.size = DATA_MESSAGE_SIZE;
    if (dispatch_(frame, 0x21))
      extra_due_ = false;
    return;
  }
  if (!commands_.empty()) {
    auto& changes = commands_.front();
    if (protocol::elapsed(now, changes.created_at, command_timeout_)) {
      commands_.pop();
      command_status_ = "queued_command_expired";
      return;
    }
    if (changes.needs_base && !standard_fresh()) {
      poll_due_ = true;
      return;
    }
    // Revalidate the setpoint context immediately before dispatch.
    for (uint8_t index = 38; index <= 41; ++index)
      if (changes.masks[index] &&
          (!standard_fresh() ||
           changes.context_mode[index - 38] != ((heatpump_default_message_[28] >> ((index & 1) ? 2 : 0)) & 3) ||
           changes.context_sensor[index - 38] != ((heatpump_default_message_[22] >> (index <= 39 ? 0 : 4)) & 15) ||
           !validate_setpoint(index, int(changes.values[index]) - 128, -128, 127))) {
        commands_.pop();
        command_status_ = "setpoint_context_changed";
        return;
      }
    const std::vector<uint8_t> empty;
    if (!changes.build(standard_fresh() ? heatpump_default_message_ : empty, frame))
      return;
    if (dispatch_(frame, 0xF1)) {
      active_changes_ = changes;
      commands_.pop();
      command_active_ = true;
      active_sent_ = false;
      command_status_ = "sent_to_transport";
    }
    return;
  }
  if (extra_due_) {
    std::copy_n(PanasonicCommand::PollingExtraMessage, DATA_MESSAGE_SIZE, frame.data.begin());
    frame.size = DATA_MESSAGE_SIZE;
    if (dispatch_(frame, 0x21))
      extra_due_ = false;
  } else if (optional_due_) {
    std::copy_n(PanasonicCommand::OptionalPCBQuery, OPTIONAL_REQUEST_SIZE, frame.data.begin());
    frame.size = OPTIONAL_REQUEST_SIZE;
    if (dispatch_(frame, 0x50))
      optional_due_ = false;
  }
}
bool PanasonicHeatpumpComponent::validate_setpoint(uint8_t index, float value, float minimum, float maximum,
                                                   float step) const {
  if (!protocol::valid_number(value, minimum, maximum, step))
    return false;
  // Bound the signed value before any conversion into the protocol byte.
  // Encoded zero is a no-op, so it cannot represent a requested setpoint.
  const bool plus_one = index == 45 || index == 104;
  if (!protocol::valid_number(value, plus_one ? 0 : -127, plus_one ? 254 : 127))
    return false;
  if (index >= 38 && index <= 41) {
    if (!standard_fresh())
      return false;
    const bool cooling = index == 39 || index == 41;
    const uint8_t mode = (heatpump_default_message_[28] >> (cooling ? 2 : 0)) & 3;
    const uint8_t sensor = (heatpump_default_message_[22] >> (index <= 39 ? 0 : 4)) & 15;
    // Water/thermostat modes only. A room thermistor has a different setpoint semantic.
    if (sensor != 1 && sensor != 2)
      return false;
    if (mode == 1)
      return protocol::valid_number(value, -5, 5);
    if (mode == 2)
      return protocol::valid_number(value, cooling ? 5 : 20, cooling ? 20 : 60);
    return false;
  }
  if (!k_profile_)
    return true;
  switch (index) {
  case 99:
    return protocol::valid_number(value, -12, -2);
  case 105:
    return protocol::valid_number(value, -10, -2);
  case 106:
    return protocol::valid_number(value, -8, 0);
  case 104:
    return protocol::valid_number(value, 10, 60);
  case 45:
    return protocol::valid_number(value, 64, 254);
  default:
    return true;
  }
}
bool PanasonicHeatpumpComponent::queue_fields(const std::vector<protocol::Field>& fields) {
  auto validated = fields;
  for (auto& field : validated) {
    if (field.mask == 0xFF && ((field.index >= 38 && field.index <= 41) || field.index == 99 || field.index == 104 ||
                               field.index == 105 || field.index == 106 || field.index == 45)) {
      const int offset = field.index == 45 || field.index == 104 ? 1 : 128;
      if (!validate_setpoint(field.index, int(field.value) - offset, -128, 254)) {
        command_status_ = "invalid_setpoint";
        return false;
      }
    }
    if (field.index >= 38 && field.index <= 41) {
      if (!standard_fresh()) {
        command_status_ = "missing_control_context";
        return false;
      }
      field.context_mode = (heatpump_default_message_[28] >> ((field.index & 1) ? 2 : 0)) & 3;
      field.context_sensor = (heatpump_default_message_[22] >> (field.index <= 39 ? 0 : 4)) & 15;
    }
  }
  if (is_failed() || !commands_.enqueue(validated, millis())) {
    command_status_ = "queue_rejected";
    ESP_LOGW(TAG, "Command rejected (invalid field, failed component or full queue)");
    return false;
  }
  max_command_queue_ = std::max(max_command_queue_, uint32_t(commands_.size()));
  command_status_ = "queued";
  return true;
}
bool PanasonicHeatpumpComponent::set_command_field(uint8_t value, uint8_t index, uint8_t mask, bool impulse,
                                                   bool needs_base) {
  return queue_fields({{index, value, mask, false, impulse, needs_base}});
}
bool PanasonicHeatpumpComponent::set_command_byte(uint8_t value, uint8_t index) {
  return set_command_field(value, index, 0xFF);
}
bool PanasonicHeatpumpComponent::set_command_curve(uint8_t value, uint8_t index) {
  const bool valid = (index >= 75 && index <= 82) || (index >= 86 && index <= 93);
  return valid && queue_fields({{index, value, 0xFF, true, false, true}});
}
std::string PanasonicHeatpumpComponent::get_model_signature() const {
  if (heatpump_default_message_.size() != 203)
    return "unknown";
  return PanasonicHelpers::byte_array_to_hex_string(heatpump_default_message_.data() + 129, 10, ' ');
}
std::string PanasonicHeatpumpComponent::get_diagnostics() const {
  // Keep the text suitable for a Home Assistant state (at most 255 chars).
  char buffer[256];
  snprintf(buffer, sizeof(buffer),
           "s=%lu/%u e=%lu/%u tx=%lu rx=%lu,%lu h=%lu c=%lu at=%lu rt=%lu ov=%lu qm=%lu,%lu lm=%lu q=%u command=%s",
           (unsigned long)(standard_count_ ? (millis() - last_standard_time_) / 1000 : UINT32_MAX),
           unsigned(standard_fresh()),
           (unsigned long)(extra_count_ ? (millis() - last_extra_time_) / 1000 : UINT32_MAX), unsigned(extra_fresh()),
           (unsigned long)tx_count_, (unsigned long)standard_count_, (unsigned long)extra_count_,
           (unsigned long)bad_header_, (unsigned long)bad_checksum_, (unsigned long)parser_timeouts_,
           (unsigned long)response_timeouts_, (unsigned long)dropped_events_.load(),
           (unsigned long)rx_queue_high_water_.load(), (unsigned long)max_command_queue_, (unsigned long)max_loop_ms_,
           unsigned(commands_.size()), command_status_.c_str());
  return buffer;
}
}  // namespace panasonic_heatpump
}  // namespace esphome
