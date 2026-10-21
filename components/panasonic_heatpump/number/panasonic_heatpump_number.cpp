#include "panasonic_heatpump_number.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump.number";

void PanasonicHeatpumpNumber::dump_config() {
  LOG_NUMBER("", "Panasonic Heatpump Number", this);
}

void PanasonicHeatpumpNumber::control(float value) {
  if (!protocol::valid_number(value, this->traits.get_min_value(), this->traits.get_max_value(),
                              this->traits.get_step())) {
    ESP_LOGW(TAG, "Invalid number value or step");
    return;
  }
  switch (this->id_) {
  case NumberIds::CONF_SET5:
    if (!this->parent_->validate_setpoint(38, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 38);
    break;
  case NumberIds::CONF_SET6:
    if (!this->parent_->validate_setpoint(39, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 39);
    break;
  case NumberIds::CONF_SET7:
    if (!this->parent_->validate_setpoint(40, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 40);
    break;
  case NumberIds::CONF_SET8:
    if (!this->parent_->validate_setpoint(41, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 41);
    break;
  case NumberIds::CONF_SET11:
    if (!this->parent_->validate_setpoint(42, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 42);
    break;
  case NumberIds::CONF_SET15:
    if (!this->parent_->validate_setpoint(45, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus1(static_cast<int>(value)), 45);
    break;
  case NumberIds::CONF_SET16_01:
    if (!this->parent_->validate_setpoint(75, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 75);
    break;
  case NumberIds::CONF_SET16_02:
    if (!this->parent_->validate_setpoint(76, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 76);
    break;
  case NumberIds::CONF_SET16_03:
    if (!this->parent_->validate_setpoint(77, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 77);
    break;
  case NumberIds::CONF_SET16_04:
    if (!this->parent_->validate_setpoint(78, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 78);
    break;
  case NumberIds::CONF_SET16_05:
    if (!this->parent_->validate_setpoint(79, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 79);
    break;
  case NumberIds::CONF_SET16_06:
    if (!this->parent_->validate_setpoint(80, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 80);
    break;
  case NumberIds::CONF_SET16_07:
    if (!this->parent_->validate_setpoint(81, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 81);
    break;
  case NumberIds::CONF_SET16_08:
    if (!this->parent_->validate_setpoint(82, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 82);
    break;
  case NumberIds::CONF_SET16_09:
    if (!this->parent_->validate_setpoint(86, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 86);
    break;
  case NumberIds::CONF_SET16_10:
    if (!this->parent_->validate_setpoint(87, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 87);
    break;
  case NumberIds::CONF_SET16_11:
    if (!this->parent_->validate_setpoint(88, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 88);
    break;
  case NumberIds::CONF_SET16_12:
    if (!this->parent_->validate_setpoint(89, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 89);
    break;
  case NumberIds::CONF_SET16_13:
    if (!this->parent_->validate_setpoint(90, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 90);
    break;
  case NumberIds::CONF_SET16_14:
    if (!this->parent_->validate_setpoint(91, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 91);
    break;
  case NumberIds::CONF_SET16_15:
    if (!this->parent_->validate_setpoint(92, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 92);
    break;
  case NumberIds::CONF_SET16_16:
    if (!this->parent_->validate_setpoint(93, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_curve(PanasonicCommand::setPlus128(static_cast<int>(value)), 93);
    break;
  case NumberIds::CONF_SET18:
    if (!this->parent_->validate_setpoint(84, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 84);
    break;
  case NumberIds::CONF_SET19:
    if (!this->parent_->validate_setpoint(94, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 94);
    break;
  case NumberIds::CONF_SET20:
    if (!this->parent_->validate_setpoint(99, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 99);
    break;
  case NumberIds::CONF_SET21:
    if (!this->parent_->validate_setpoint(104, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus1(static_cast<int>(value)), 104);
    break;
  case NumberIds::CONF_SET22:
    if (!this->parent_->validate_setpoint(105, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 105);
    break;
  case NumberIds::CONF_SET23:
    if (!this->parent_->validate_setpoint(106, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 106);
    break;
  case NumberIds::CONF_SET27:
    if (!this->parent_->validate_setpoint(59, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 59);
    break;
  case NumberIds::CONF_SET29:
    if (!this->parent_->validate_setpoint(83, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 83);
    break;
  case NumberIds::CONF_SET36:
    if (!this->parent_->validate_setpoint(65, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 65);
    break;
  case NumberIds::CONF_SET37:
    if (!this->parent_->validate_setpoint(66, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 66);
    break;
  case NumberIds::CONF_SET38:
    if (!this->parent_->validate_setpoint(68, value, this->traits.get_min_value(), this->traits.get_max_value(),
                                          this->traits.get_step())) {
      ESP_LOGW(TAG, "Setpoint rejected by profile or current control context");
      return;
    }
    this->parent_->set_command_byte(PanasonicCommand::setPlus128(static_cast<int>(value)), 68);
    break;
  default:
    return;
  };
}

void PanasonicHeatpumpNumber::publish_new_state(const std::vector<uint8_t>& data) {
  if (data.empty())
    return;

  auto require_index = [&](size_t index) {
    if (data.size() <= index) {
      ESP_LOGW(TAG, "Data too short (%zu) for index %zu", data.size(), index);
      return false;
    }
    return true;
  };

  float new_state;
  switch (this->id_) {
  case NumberIds::CONF_SET11:
    if (!require_index(42))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[42]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET20:
    if (!require_index(99))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[99]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET18:
    if (!require_index(84))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[84]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET19:
    if (!require_index(94))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[94]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET5:
    if (!require_index(38))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[38]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET6:
    if (!require_index(39))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[39]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_01:
    if (!require_index(75))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[75]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_02:
    if (!require_index(76))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[76]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_04:
    if (!require_index(78))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[78]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_03:
    if (!require_index(77))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[77]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET7:
    if (!require_index(40))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[40]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET8:
    if (!require_index(41))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[41]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_09:
    if (!require_index(86))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[86]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_10:
    if (!require_index(87))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[87]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_12:
    if (!require_index(89))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[89]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_11:
    if (!require_index(88))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[88]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET29:
    if (!require_index(83))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[83]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_05:
    if (!require_index(79))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[79]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_06:
    if (!require_index(80))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[80]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_08:
    if (!require_index(82))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[82]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_07:
    if (!require_index(81))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[81]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_13:
    if (!require_index(90))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[90]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_14:
    if (!require_index(91))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[91]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_16:
    if (!require_index(93))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[93]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET16_15:
    if (!require_index(92))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[92]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET15:
    if (!require_index(45))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[45]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET21:
    if (!require_index(104))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[104]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET22:
    if (!require_index(105))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[105]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET23:
    if (!require_index(106))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[106]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET27:
    if (!require_index(59))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[59]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET36:
    if (!require_index(65))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[65]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET37:
    if (!require_index(66))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[66]);
    if (this->has_state() && this->state == new_state)
      return;
    break;
  case NumberIds::CONF_SET38:
    if (!require_index(68))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[68]);
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
