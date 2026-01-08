#include "panasonic_heatpump_text_sensor.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump.text_sensor";

void PanasonicHeatpumpTextSensor::dump_config() {
  LOG_TEXT_SENSOR("", "Panasonic Heatpump Text Sensor", this);
  delay(10);
}

void PanasonicHeatpumpTextSensor::publish_new_state(const std::vector<uint8_t>& data) {
  if (data.empty())
    return;

  auto require_index = [&](size_t index) {
    if (data.size() <= index) {
      ESP_LOGW(TAG, "Data too short (%zu) for index %zu", data.size(), index);
      return false;
    }
    return true;
  };

  std::string new_state;
  switch (this->id_) {
  case TextSensorIds::CONF_TOP4:
    if (!require_index(6))
      return;
    new_state =
        PanasonicDecode::getTextState(PanasonicDecode::OperationMode, PanasonicDecode::getOperationMode(data[6]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP17:
    if (!require_index(7))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::PowerfulMode, PanasonicDecode::getBit6and7and8(data[7]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP18:
    if (!require_index(7))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::QuietMode, PanasonicDecode::getBit3and4and5(data[7]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP19:
    if (!require_index(5))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::HolidayState, PanasonicDecode::getBit3and4(data[5]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP20:
    if (!require_index(111))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::ThreeWayValve, PanasonicDecode::getBit7and8(data[111]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP44:
    if (!require_index(114))
      return;
    new_state = PanasonicDecode::getErrorInfo(data[113], data[114]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP58:
    if (!require_index(9))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::BlockedFree, PanasonicDecode::getBit5and6(data[9]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP59:
    if (!require_index(9))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::BlockedFree, PanasonicDecode::getBit7and8(data[9]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP76:
    if (!require_index(28))
      return;
    new_state =
        PanasonicDecode::getTextState(PanasonicDecode::WaterTempControl, PanasonicDecode::getBit7and8(data[28]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP81:
    if (!require_index(28))
      return;
    new_state =
        PanasonicDecode::getTextState(PanasonicDecode::WaterTempControl, PanasonicDecode::getBit5and6(data[28]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP92:
    if (!require_index(138))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::ModelNames, PanasonicDecode::getModel(data, 129));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP94:
    if (!require_index(6))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::ZoneState, PanasonicDecode::getBit1and2(data[6]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP101:
    if (!require_index(24))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::SolarMode, PanasonicDecode::getBit3and4(data[24]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP106:
    if (!require_index(29))
      return;
    new_state =
        PanasonicDecode::getTextState(PanasonicDecode::PumpFlowRateMode, PanasonicDecode::getBit3and4(data[29]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP107:
    if (!require_index(20))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::LiquidType, PanasonicDecode::getBit1(data[20]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP111:
    if (!require_index(22))
      return;
    new_state =
        PanasonicDecode::getTextState(PanasonicDecode::ZoneSensorType, PanasonicDecode::getLowNibbleMinus1(data[22]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP112:
    if (!require_index(22))
      return;
    new_state =
        PanasonicDecode::getTextState(PanasonicDecode::ZoneSensorType, PanasonicDecode::getHighNibbleMinus1(data[22]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP114:
    if (!require_index(25))
      return;
    new_state =
        PanasonicDecode::getTextState(PanasonicDecode::ExtPadHeaterType, PanasonicDecode::getBit3and4(data[25]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP125:
    if (!require_index(116))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::TwoWayValve, PanasonicDecode::getBit5and6(data[116]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP126:
    if (!require_index(116))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::ThreeWayValve, PanasonicDecode::getBit7and8(data[116]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case TextSensorIds::CONF_TOP130:
    if (!require_index(26))
      return;
    new_state = PanasonicDecode::getTextState(PanasonicDecode::BivalentMode, PanasonicDecode::getBit5and6(data[26]));
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  default:
    return;
  };

  this->publish_state(new_state);
}
}  // namespace panasonic_heatpump
}  // namespace esphome
