#pragma once
#include <cstdint>
#include <cstddef>
namespace esphome {
namespace uart {
enum class UARTFlushResult {
  UART_FLUSH_RESULT_SUCCESS,
  UART_FLUSH_RESULT_TIMEOUT,
  UART_FLUSH_RESULT_FAILED,
  UART_FLUSH_RESULT_ASSUMED_SUCCESS
};
class UARTComponent {
 public:
  virtual ~UARTComponent() = default;
  virtual void write_array(const uint8_t*, size_t) = 0;
  virtual bool read_byte(uint8_t*) = 0;
  virtual size_t available() = 0;
  virtual UARTFlushResult flush() {
    return UARTFlushResult::UART_FLUSH_RESULT_SUCCESS;
  }
  void set_flush_timeout(uint32_t) {
  }
  void write_byte(uint8_t value) {
    write_array(&value, 1);
  }
};
class UARTDevice {
 public:
  void set_uart_parent(UARTComponent* parent) {
    parent_ = parent;
  }
  void write_array(const uint8_t* data, size_t size) {
    parent_->write_array(data, size);
  }
  bool read_byte(uint8_t* byte) {
    return parent_->read_byte(byte);
  }
  size_t available() {
    return parent_->available();
  }
  UARTFlushResult flush() {
    return parent_->flush();
  }

 protected:
  UARTComponent* parent_{nullptr};
};
}  // namespace uart
}  // namespace esphome
