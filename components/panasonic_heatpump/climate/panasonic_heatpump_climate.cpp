#include "panasonic_heatpump_climate.h"
#include "esphome/core/log.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump.climate";

void PanasonicHeatpumpClimate::dump_config() {
  LOG_CLIMATE("", "Panasonic Heatpump Climate", this);
}

climate::ClimateTraits PanasonicHeatpumpClimate::traits() {
  auto traits = climate::ClimateTraits();

  // traits.set_supports_action(true);
  traits.add_feature_flags(climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE);
  traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT});
  if (this->cool_mode_ &&
      (this->id_ == ClimateIds::CONF_CLIMATE_ZONE1 || this->id_ == ClimateIds::CONF_CLIMATE_ZONE2)) {
    traits.add_feature_flags(climate::CLIMATE_REQUIRES_TWO_POINT_TARGET_TEMPERATURE);
    traits.add_supported_mode(climate::CLIMATE_MODE_COOL);
    traits.add_supported_mode(climate::CLIMATE_MODE_AUTO);
  }

  traits.set_visual_min_temperature(this->min_temperature_);
  traits.set_visual_max_temperature(this->max_temperature_);
  traits.set_visual_temperature_step(this->temperature_step_);

  return traits;
}

void PanasonicHeatpumpClimate::control(const climate::ClimateCall& call) {
  std::vector<protocol::Field> fields;
  auto add_target = [&](uint8_t index, float value) -> bool {
    if (!this->parent_->validate_setpoint(index, value, this->min_temperature_, this->max_temperature_,
                                          this->temperature_step_))
      return false;
    fields.push_back({index, PanasonicCommand::setPlus128(static_cast<int>(value)), 0xFF});
    return true;
  };
  if (call.get_mode().has_value()) {
    const auto mode = *call.get_mode();
    const bool supported = mode == climate::CLIMATE_MODE_OFF || mode == climate::CLIMATE_MODE_HEAT ||
                           (this->id_ != ClimateIds::CONF_CLIMATE_TANK && this->cool_mode_ &&
                            (mode == climate::CLIMATE_MODE_COOL || mode == climate::CLIMATE_MODE_AUTO));
    const int current = this->parent_->getResponseByte(6);
    if (!supported || current < 0) {
      ESP_LOGW(TAG, "Climate mode requires a supported mode and fresh base");
      return;
    }
    const uint8_t mask = this->id_ == ClimateIds::CONF_CLIMATE_TANK ? 0x30 : 0x0F;
    fields.push_back({6, this->setClimateMode(mode, current), mask, false, false, true});
  }
  bool valid = true;
  if (this->id_ == ClimateIds::CONF_CLIMATE_TANK) {
    if (call.get_target_temperature_high().has_value() || call.get_target_temperature_low().has_value())
      valid = false;
    if (call.get_target_temperature().has_value())
      valid &= add_target(42, *call.get_target_temperature());
  } else {
    const uint8_t heat_index = this->id_ == ClimateIds::CONF_CLIMATE_ZONE1 ? 38 : 40;
    if (call.get_target_temperature().has_value())
      valid &= add_target(heat_index, *call.get_target_temperature());
    if (call.get_target_temperature_high().has_value())
      valid &= add_target(heat_index, *call.get_target_temperature_high());
    if (call.get_target_temperature_low().has_value())
      valid &= this->cool_mode_ && add_target(heat_index + 1, *call.get_target_temperature_low());
  }
  if (!valid) {
    ESP_LOGW(TAG, "Entire climate request rejected: invalid value, step or control context");
    return;
  }
  if (!fields.empty())
    this->parent_->queue_fields(fields);
}

void PanasonicHeatpumpClimate::publish_new_state(const std::vector<uint8_t>& data) {
  if (data.empty())
    return;

  auto require_index = [&](size_t index) {
    if (data.size() <= index) {
      ESP_LOGW(TAG, "Data too short (%zu) for index %zu", data.size(), index);
      return false;
    }
    return true;
  };

  uint8_t new_mode;
  float new_target_temp_heat = this->target_temperature;
  float new_target_temp_cool = NAN;
  float new_current_temp = NAN;

  if (!require_index(6))
    return;
  new_mode = this->getClimateMode(data[6]);  // set9
  switch (this->id_) {
  case ClimateIds::CONF_CLIMATE_TANK:
    if (!require_index(141))
      return;
    new_target_temp_heat = PanasonicDecode::getByteMinus128(data[42]);  // set11
    new_current_temp = PanasonicDecode::getByteMinus128(data[141]);     // top10
    break;
  case ClimateIds::CONF_CLIMATE_ZONE1:
    if (!require_index(139))
      return;
    new_target_temp_heat = PanasonicDecode::getByteMinus128(data[38]);  // set5
    new_target_temp_cool = PanasonicDecode::getByteMinus128(data[39]);  // set6
    new_current_temp = PanasonicDecode::getByteMinus128(data[139]);     // top56
    break;
  case ClimateIds::CONF_CLIMATE_ZONE2:
    if (!require_index(140))
      return;
    new_target_temp_heat = PanasonicDecode::getByteMinus128(data[40]);  // set7
    new_target_temp_cool = PanasonicDecode::getByteMinus128(data[41]);  // set8
    new_current_temp = PanasonicDecode::getByteMinus128(data[140]);     // top57
    break;
  default:
    return;
  };

  if (!this->get_traits().has_feature_flags(climate::CLIMATE_REQUIRES_TWO_POINT_TARGET_TEMPERATURE) &&
      this->mode == new_mode && this->target_temperature == new_target_temp_heat &&
      this->current_temperature == new_current_temp)
    return;
  if (this->get_traits().has_feature_flags(climate::CLIMATE_REQUIRES_TWO_POINT_TARGET_TEMPERATURE) &&
      this->mode == new_mode && this->target_temperature_high == new_target_temp_heat &&
      this->target_temperature_low == new_target_temp_cool && this->current_temperature == new_current_temp)
    return;

  if (new_mode != 255)
    this->mode = (climate::ClimateMode)new_mode;
  // this->action = climate::CLIMATE_ACTION_IDLE;
  if (this->get_traits().has_feature_flags(climate::CLIMATE_REQUIRES_TWO_POINT_TARGET_TEMPERATURE)) {
    this->target_temperature_high = new_target_temp_heat;
    this->target_temperature_low = new_target_temp_cool;
  } else {
    this->target_temperature = new_target_temp_heat;
  }
  this->current_temperature = new_current_temp;
  this->publish_state();
}

uint8_t PanasonicHeatpumpClimate::getClimateMode(const uint8_t input) {
  switch (this->id_) {
  case ClimateIds::CONF_CLIMATE_TANK:
    switch ((uint8_t)(input & 0b110000)) {
    case 0b010000:
      return climate::CLIMATE_MODE_OFF;
    case 0b100000:
      return climate::CLIMATE_MODE_HEAT;
    default:
      return 255;
    };
  case ClimateIds::CONF_CLIMATE_ZONE1:
  case ClimateIds::CONF_CLIMATE_ZONE2:
    switch ((uint8_t)(input & 0b1111)) {
    case 0b0001:
      return climate::CLIMATE_MODE_OFF;
    case 0b0010:
      return climate::CLIMATE_MODE_HEAT;
    case 0b0011:
      return climate::CLIMATE_MODE_COOL;
    case 0b1000:
      return climate::CLIMATE_MODE_AUTO;
    case 0b1001:
      return climate::CLIMATE_MODE_AUTO;  // 0x19 = auto-heat
    case 0b1010:
      return climate::CLIMATE_MODE_AUTO;  // 0x1A = auto-cool
    default:
      return 255;
    };
  default:
    return 255;
  };
}

uint8_t PanasonicHeatpumpClimate::setClimateMode(const climate::ClimateMode mode, uint8_t byte) {
  uint8_t newByte = byte;
  switch (this->id_) {
  case ClimateIds::CONF_CLIMATE_TANK:
    newByte = newByte & 0b11001111;
    switch (mode) {
    case climate::CLIMATE_MODE_OFF:
      return newByte + 0b010000;
    case climate::CLIMATE_MODE_HEAT:
      return newByte + 0b100000;
    default:
      return 0;
    };
  case ClimateIds::CONF_CLIMATE_ZONE1:
  case ClimateIds::CONF_CLIMATE_ZONE2:
    newByte = newByte & 0b11110000;
    switch (mode) {
    case climate::CLIMATE_MODE_OFF:
      return newByte + 0b0001;
    case climate::CLIMATE_MODE_HEAT:
      return newByte + 0b0010;
    case climate::CLIMATE_MODE_COOL:
      return newByte + 0b0011;
    case climate::CLIMATE_MODE_AUTO:
      return newByte + 0b1000;
    default:
      return 0;
    };
  default:
    return 0;
  };
}
}  // namespace panasonic_heatpump
}  // namespace esphome
