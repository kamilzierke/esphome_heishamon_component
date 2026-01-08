#include "panasonic_heatpump_binary_sensor.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump.binary_sensor";

void PanasonicHeatpumpBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "Panasonic Heatpump Binary Sensor", this);
  delay(10);
}

void PanasonicHeatpumpBinarySensor::publish_new_state(const std::vector<uint8_t>& data) {
  if (data.empty())
    return;

  auto require_index = [&](size_t index) {
    if (data.size() <= index) {
      ESP_LOGW(TAG, "Data too short (%zu) for index %zu", data.size(), index);
      return false;
    }
    return true;
  };

  bool new_state;
  switch (this->id_) {
  case BinarySensorIds::CONF_UART_CLIENT_TIMED_OUT:
    new_state = this->parent_->get_uart_client_timeout_exceeded();
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP0:
    if (!require_index(4))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[4]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP2:
    if (!require_index(4))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[4]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP3:
    if (!require_index(7))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[7]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP13:
    if (!require_index(5))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[5]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP26:
    if (!require_index(111))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[111]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP60:
    if (!require_index(112))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[112]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP61:
    if (!require_index(112))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[112]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP68:
    if (!require_index(5))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[5]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP69:
    if (!require_index(117))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[117]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP99:
    if (!require_index(24))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[24]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP100:
    if (!require_index(24))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[24]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP108:
    if (!require_index(20))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit3and4(data[20]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP109:
    if (!require_index(20))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[20]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP110:
    if (!require_index(20))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[20]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP119:
    if (!require_index(23))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[23]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP120:
    if (!require_index(23))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit5and6(data[23]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP121:
    if (!require_index(23))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit3and4(data[23]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP122:
    if (!require_index(23))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[23]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP123:
    if (!require_index(116))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[116]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP124:
    if (!require_index(116))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit3and4(data[116]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP129:
    if (!require_index(26))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit7and8(data[26]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP132:
    if (!require_index(26))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit3and4(data[26]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_TOP133:
    if (!require_index(26))
      return;
    new_state = PanasonicDecode::getBinaryState(PanasonicDecode::getBit1and2(data[26]));
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_OPT0:
    if (!require_index(4))
      return;
    new_state = (data[4] >> 7) != 0;
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_OPT2:
    if (!require_index(4))
      return;
    new_state = ((data[4] >> 4) & 0b1) != 0;
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_OPT4:
    if (!require_index(4))
      return;
    new_state = ((data[4] >> 1) & 0b1) != 0;
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_OPT5:
    if (!require_index(4))
      return;
    new_state = (data[4] & 0b1) != 0;
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case BinarySensorIds::CONF_OPT6:
    if (!require_index(5))
      return;
    new_state = (data[5] & 0b1) != 0;
    if (this->has_state() && this->state == new_state)
      return;
    break;
  default:
    return;
  };

  this->publish_state(new_state);
}
}  // namespace panasonic_heatpump
}  // namespace esphome
