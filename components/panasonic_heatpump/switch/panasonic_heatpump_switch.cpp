#include "panasonic_heatpump_switch.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump.switch";

void PanasonicHeatpumpSwitch::dump_config() {
  LOG_SWITCH("", "Panasonic Heatpump Switch", this);
}

void PanasonicHeatpumpSwitch::write_state(bool state) {
  int value = state ? 1 : 0;

  if (!state && (this->id_ == SwitchIds::CONF_SET12 || this->id_ == SwitchIds::CONF_SET13))
    return;
  switch (this->id_) {
  case SwitchIds::CONF_SET1:
    this->parent_->set_command_field(PanasonicCommand::setPlus1(value), 4, 0x03);
    break;
  case SwitchIds::CONF_SET10:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply64(value), 4, 0xC0);
    break;
  case SwitchIds::CONF_SET12:
    this->parent_->set_command_field(PanasonicCommand::setMultiply2(value), 8, 0x02, true);
    break;
  case SwitchIds::CONF_SET13:
    this->parent_->set_command_field(PanasonicCommand::setMultiply4(value), 8, 0x04, true);
    break;
  case SwitchIds::CONF_SET14:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply16(value), 4, 0x30);
    break;
  case SwitchIds::CONF_SET24:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply64(value), 5, 0xC0);
    break;
  case SwitchIds::CONF_SET25:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply16(value), 20, 0x30);
    break;
  case SwitchIds::CONF_SET28:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply4(value), 24, 0x0C);
    break;
  case SwitchIds::CONF_SET30:
    this->parent_->set_command_field(PanasonicCommand::setPlus1(value), 23, 0x03);
    break;
  case SwitchIds::CONF_SET31:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply16(value), 23, 0x30);
    break;
  case SwitchIds::CONF_SET32:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply64(value), 23, 0xC0);
    break;
  case SwitchIds::CONF_SET33:
    this->parent_->set_command_field(PanasonicCommand::setPlus1Multiply4(value), 23, 0x0C);
    break;
  case SwitchIds::CONF_SET34:
    this->parent_->set_command_field(PanasonicCommand::setPlus1(value), 26, 0x03);
    break;
  default:
    return;
  };
}

void PanasonicHeatpumpSwitch::publish_new_state(const std::vector<uint8_t>& data) {
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
  case SwitchIds::CONF_SET1:
    if (!require_index(4))
      return;
    {
      const int decoded = PanasonicDecode::getBit7and8(data[4]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET10:
    if (!require_index(4))
      return;
    {
      const int decoded = PanasonicDecode::getBit1and2(data[4]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET14:
    // Byte 4, bits 3-4 (ProtocolByteDecrypt.md: SET14)
    {
      const int decoded = PanasonicDecode::getBit3and4(data[4]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET24:
    if (!require_index(5))
      return;
    {
      const int decoded = PanasonicDecode::getBit1and2(data[5]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET12:
    if (!require_index(111))
      return;
    {
      const int decoded = PanasonicDecode::getBit5and6(data[111]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET13:
    if (!require_index(117))
      return;
    {
      const int decoded = PanasonicDecode::getBit5and6(data[117]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET28:
    if (!require_index(24))
      return;
    {
      const int decoded = PanasonicDecode::getBit5and6(data[24]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET25:
    if (!require_index(20))
      return;
    {
      const int decoded = PanasonicDecode::getBit3and4(data[20]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET30:
    if (!require_index(23))
      return;
    {
      const int decoded = PanasonicDecode::getBit7and8(data[23]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET33:
    if (!require_index(23))
      return;
    {
      const int decoded = PanasonicDecode::getBit5and6(data[23]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET31:
    if (!require_index(23))
      return;
    {
      const int decoded = PanasonicDecode::getBit3and4(data[23]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET32:
    if (!require_index(23))
      return;
    {
      const int decoded = PanasonicDecode::getBit1and2(data[23]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case SwitchIds::CONF_SET34:
    if (!require_index(26))
      return;
    {
      const int decoded = PanasonicDecode::getBit7and8(data[26]);
      if (decoded != 0 && decoded != 1) {
        this->invalidate();
        return;
      }
      new_state = decoded == 1;
    }
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
