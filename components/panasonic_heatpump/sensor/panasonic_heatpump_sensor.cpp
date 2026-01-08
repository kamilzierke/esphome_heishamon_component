#include "panasonic_heatpump_sensor.h"

namespace esphome {
namespace panasonic_heatpump {
static const char* const TAG = "panasonic_heatpump.sensor";

void PanasonicHeatpumpSensor::dump_config() {
  LOG_SENSOR("", "Panasonic Heatpump Sensor", this);
  delay(10);
}

void PanasonicHeatpumpSensor::publish_new_state(const std::vector<uint8_t>& data) {
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
  case SensorIds::CONF_TOP1:
    if (!require_index(170))
      return;
    new_state = PanasonicDecode::getPumpFlow(data, 169);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP5:
    if (!require_index(143))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[143]) + PanasonicDecode::getFractional(data[118], 0);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP6:
    if (!require_index(144))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[144]) + PanasonicDecode::getFractional(data[118], 3);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP7:
    if (!require_index(153))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[153]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP8:
    if (!require_index(166))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[166]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP9:
    if (!require_index(42))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[42]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP10:
    if (!require_index(141))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[141]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP11:
    if (!require_index(183))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 182);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP12:
    if (!require_index(180))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 179);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP14:
    if (!require_index(142))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[142]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP15:
    if (!require_index(194))
      return;
    new_state = PanasonicDecode::getByteMinus1Times200(data[194]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP16:
    if (!require_index(193))
      return;
    new_state = PanasonicDecode::getByteMinus1Times200(data[193]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP21:
    if (!require_index(158))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[158]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP22:
    if (!require_index(99))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[99]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP23:
    if (!require_index(84))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[84]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP24:
    if (!require_index(94))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[94]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP25:
    if (!require_index(44))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[44]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP27:
    if (!require_index(38))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[38]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP28:
    if (!require_index(39))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[39]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP29:
    if (!require_index(75))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[75]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP30:
    if (!require_index(76))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[76]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP31:
    if (!require_index(78))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[78]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP32:
    if (!require_index(77))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[77]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP33:
    if (!require_index(156))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[156]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP34:
    if (!require_index(40))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[40]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP35:
    if (!require_index(41))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[41]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP36:
    if (!require_index(145))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[145]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP37:
    if (!require_index(146))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[146]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP38:
    if (!require_index(196))
      return;
    new_state = PanasonicDecode::getByteMinus1Times200(data[196]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP39:
    if (!require_index(195))
      return;
    new_state = PanasonicDecode::getByteMinus1Times200(data[195]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP40:
    if (!require_index(198))
      return;
    new_state = PanasonicDecode::getByteMinus1Times200(data[198]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP41:
    if (!require_index(197))
      return;
    new_state = PanasonicDecode::getByteMinus1Times200(data[197]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP42:
    if (!require_index(147))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[147]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP43:
    if (!require_index(148))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[148]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP45:
    if (!require_index(43))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[43]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP46:
    if (!require_index(149))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[149]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP47:
    if (!require_index(150))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[150]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP48:
    if (!require_index(151))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[151]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP49:
    if (!require_index(154))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[154]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP50:
    if (!require_index(155))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[155]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP51:
    if (!require_index(157))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[157]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP52:
    if (!require_index(159))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[159]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP53:
    if (!require_index(160))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[160]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP54:
    if (!require_index(161))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[161]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP55:
    if (!require_index(162))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[162]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP56:
    if (!require_index(139))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[139]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP57:
    if (!require_index(140))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[140]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP62:
    if (!require_index(173))
      return;
    new_state = PanasonicDecode::getByteMinus1Times10(data[173]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP63:
    if (!require_index(174))
      return;
    new_state = PanasonicDecode::getByteMinus1Times10(data[174]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP64:
    if (!require_index(163))
      return;
    new_state = PanasonicDecode::getByteMinus1Div5(data[163]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP65:
    if (!require_index(171))
      return;
    new_state = PanasonicDecode::getByteMinus1Times50(data[171]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP66:
    if (!require_index(164))
      return;
    new_state = PanasonicDecode::getByteMinus1Times50(data[164]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP67:
    if (!require_index(165))
      return;
    new_state = PanasonicDecode::getByteMinus1Div5(data[165]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP70:
    if (!require_index(100))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[100]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP71:
    if (!require_index(101))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[101]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP72:
    if (!require_index(86))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[86]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP73:
    if (!require_index(87))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[87]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP74:
    if (!require_index(89))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[89]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP75:
    if (!require_index(88))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[88]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP77:
    if (!require_index(83))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[83]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP78:
    if (!require_index(85))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[85]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP79:
    if (!require_index(95))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[95]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP80:
    if (!require_index(96))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[96]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP82:
    if (!require_index(79))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[79]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP83:
    if (!require_index(80))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[80]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP84:
    if (!require_index(82))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[82]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP85:
    if (!require_index(81))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[81]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP86:
    if (!require_index(90))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[90]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP87:
    if (!require_index(91))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[91]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP88:
    if (!require_index(93))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[93]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP89:
    if (!require_index(92))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[92]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP90:
    if (!require_index(186))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 185);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP91:
    if (!require_index(189))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 188);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP93:
    if (!require_index(172))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[172]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP95:
    if (!require_index(45))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[45]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP96:
    if (!require_index(104))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[104]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP97:
    if (!require_index(105))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[105]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP98:
    if (!require_index(106))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[106]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP102:
    if (!require_index(61))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[61]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP103:
    if (!require_index(62))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[62]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP104:
    if (!require_index(63))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[63]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP105:
    if (!require_index(64))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[64]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP113:
    if (!require_index(59))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[59]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP115:
    if (!require_index(125))
      return;
    new_state = PanasonicDecode::getByteMinus1Div50(data[125]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP116:
    if (!require_index(126))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[126]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP117:
    if (!require_index(127))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[127]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP118:
    if (!require_index(128))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[128]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP127:
    if (!require_index(177))
      return;
    new_state = PanasonicDecode::getByteMinus1Div2(data[177]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP128:
    if (!require_index(178))
      return;
    new_state = PanasonicDecode::getByteMinus1Div2(data[178]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP131:
    if (!require_index(65))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[65]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP134:
    if (!require_index(66))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[66]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP135:
    if (!require_index(68))
      return;
    new_state = PanasonicDecode::getByteMinus128(data[68]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP136:
    if (!require_index(67))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[67]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP137:
    if (!require_index(69))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[69]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_TOP138:
    if (!require_index(70))
      return;
    new_state = PanasonicDecode::getByteMinus1(data[70]);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;

  case SensorIds::CONF_XTOP0:
    if (!require_index(15))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 14);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_XTOP1:
    if (!require_index(17))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 16);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_XTOP2:
    if (!require_index(19))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 18);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_XTOP3:
    if (!require_index(21))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 20);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_XTOP4:
    if (!require_index(23))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 22);
    if (this->has_state() && this->get_state() == new_state)
      return;
    break;
  case SensorIds::CONF_XTOP5:
    if (!require_index(25))
      return;
    new_state = PanasonicDecode::getWordMinus1(data, 24);
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
