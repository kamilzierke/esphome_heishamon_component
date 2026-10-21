#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include "../../components/panasonic_heatpump/reliable_protocol.h"
#include "../../components/panasonic_heatpump/commands.h"
#include "../../components/panasonic_heatpump/decode.h"
using namespace esphome::panasonic_heatpump;
using namespace esphome::panasonic_heatpump::protocol;
Frame standard(uint8_t type = 0x10) {
  Frame f;
  f.size = 203;
  f.data[0] = 0x71;
  f.data[1] = 0xC8;
  f.data[2] = 1;
  f.data[3] = type;
  f.data[202] = checksum(f.data.data(), 202);
  return f;
}
int main(int argc, char** argv) {
  if (argc == 3) {
    std::vector<std::vector<uint8_t>> fixtures;
    for (int arg = 1; arg <= 2; ++arg) {
      std::ifstream input(argv[arg]);
      assert(input.good());
      unsigned value;
      std::vector<uint8_t> data;
      while (input >> std::hex >> value) {
        assert(value <= 255);
        data.push_back(value);
      }
      assert(data.size() == 203 && checksum(data.data(), data.size()) == 0);
      Parser p;
      unsigned received = 0;
      for (size_t i = 0; i < data.size(); ++i)
        p.feed(data[i], uint32_t(i), [&](const Frame&) { ++received; });
      assert(received == 1);
      fixtures.push_back(data);
    }
    assert(fixtures[0][3] == 0x10 && fixtures[1][3] == 0x21);
    assert(PanasonicDecode::getByteMinus128(fixtures[0][99]) == -5);
    assert(PanasonicDecode::getWordMinus1(fixtures[1], 14) == 1000);  // synthetic XTOP0 consumption
    assert(PanasonicDecode::getWordMinus1(fixtures[1], 20) == 4000);  // synthetic XTOP3 production
  }
  assert(valid_number(-5, -12, -2));
  assert(!valid_number(NAN, -12, -2));
  assert(!valid_number(INFINITY, -12, -2));
  assert(!valid_number(-5.5, -12, -2));
  assert(!valid_number(0, -12, -2));
  assert(!valid_number(1, 2, 1));
  assert(!valid_number(1, 0, 2, 0));
  assert(PanasonicCommand::setPlus128(-5) == 0x7B);
  assert(PanasonicCommand::setPlus128(-1) == 0x7F);
  assert(PanasonicCommand::setPlus1(254) == 0xFF);
  for (int code = 0; code < 8; ++code) {
    const float temperature = water_temperature(150, code, 0);
    if (code >= 1 && code <= 4)
      assert(temperature == 22 + (code - 1) * 0.25f);
    else
      assert(std::isnan(temperature));
  }
  assert(water_temperature(127, 1, 0) == -1);
  assert(std::isnan(water_temperature(127, 2, 0)));
  assert(!PanasonicDecode::getBinaryState(-1));
  assert(!PanasonicDecode::getBinaryState(2));
  assert(PanasonicDecode::getBinaryState(1));
  Parser parser;
  int good = 0;
  auto accept = [&](const Frame& f) {
    assert(f.size == 203);
    ++good;
  };
  auto f = standard();
  for (size_t i = 0; i < f.size; i++)
    parser.feed(f.data[i], i, accept);
  assert(good == 1);
  for (unsigned repeat = 0; repeat < 2; ++repeat)
    for (size_t i = 0; i < f.size; i++)
      parser.feed(f.data[i], 300 + repeat * 220 + i, accept);
  assert(good == 3);  // identical payloads are new samples
  auto bad = f;
  bad.data[40] ^= 1;
  for (size_t i = 0; i < bad.size; i++)
    parser.feed(bad.data[i], 1000 + i, accept);
  for (size_t i = 0; i < f.size; i++)
    parser.feed(f.data[i], 1300 + i, accept);
  assert(good == 4 && parser.bad_checksum > 0);
  for (uint8_t length : {uint8_t(0), uint8_t(1), uint8_t(255)}) {
    parser.feed(0x71, 1700, accept);
    parser.feed(length, 1701, accept);
  }
  for (size_t i = 0; i < f.size; i++)
    parser.feed(f.data[i], 1800 + i, accept);
  assert(good == 5);
  parser.feed(0x71, 2200, accept);
  parser.feed(0xC8, 2201, accept);
  parser.tick(2500);
  assert(parser.timeouts > 0);
  for (size_t i = 0; i < f.size; i++)
    parser.feed(f.data[i], 2600 + i, accept);
  assert(good == 6);
  // A partial second packet must not erase the complete first one.
  for (size_t i = 0; i < f.size; i++)
    parser.feed(f.data[i], 3000 + i, accept);
  parser.feed(0x71, 3210, accept);
  parser.feed(0xC8, 3211, accept);
  assert(good == 7);
  // A corrupt long envelope may contain a complete short frame. Resync must
  // consume the short frame and residual bytes without overflowing the buffer.
  Parser recovery;
  auto corrupt = standard();
  std::array<uint8_t, 8> small{{0x31, 5, 1, 1, 0, 0, 0, 0}};
  small.back() = checksum(small.data(), 7);
  std::copy(small.begin(), small.end(), corrupt.data.begin() + 10);
  corrupt.data[202] ^= 0x33;
  int recovered = 0;
  for (size_t i = 0; i < corrupt.size; ++i)
    recovery.feed(corrupt.data[i], 4000 + i, [&](const Frame& packet) {
      assert(packet.size == 8 && packet.started_at == 4010 && packet.received_at == 4017);
      ++recovered;
    });
  for (size_t i = 0; i < f.size; ++i)
    recovery.feed(f.data[i], 4300 + i, [&](const Frame& packet) {
      assert(packet.size == 203);
      ++recovered;
    });
  assert(recovered == 2);
  CommandQueue q;
  assert(q.enqueue({{7, 0x10, 0x38}}, 100));
  assert(q.enqueue({{7, 0x03, 0x07}}, 101));
  assert(q.size() == 1);
  Frame command;
  std::vector<uint8_t> base(203, 128);
  assert(q.front().build(base, command));
  assert(command.data[7] == 0x13);
  assert(checksum(command.data.data(), command.size) == 0);
  base[7] = 0x13;
  assert(q.front().matches(base));
  base[7] = 0x11;
  assert(!q.front().matches(base));
  q.pop();
  assert(q.enqueue({{75, 170, 0xFF, true, false, true}, {77, 110, 0xFF, true, false, true}}, 200));
  assert(q.enqueue({{76, 160, 0xFF, true, false, true}}, 201));
  assert(!q.front().build({}, command));
  assert(q.front().build(base, command));
  assert(command.data[75] == 170 && command.data[76] == 160 && command.data[77] == 110);
  assert(command.data[78] == 128 && command.data[86] == 128);
  q.pop();
  for (int i = 0; i < 8; ++i)
    assert(q.enqueue({{8, 2, 2, false, true}}, 300 + i));
  assert(!q.enqueue({{8, 2, 2, false, true}}, 400));
  const auto split = split_at_midnight(5.0 / 3600, 5000, 2000);
  assert(std::abs(split.previous - 3.0 / 3600) < 1e-12);
  assert(std::abs(split.current - 2.0 / 3600) < 1e-12);
  assert(split_at_midnight(1, 5000, 6000).current == 1);
  EnergyWindow energy;
  double sum = 0, kwh;
  uint32_t dt;
  for (uint32_t ms = 0; ms <= 3600000; ms += 5000) {
    energy.push(1000, ms, kwh, dt);
    sum += kwh;
  }
  assert(std::abs(sum - 1) < 1e-10);
  energy.push(1000, 3700000, kwh, dt);
  assert(kwh == 0 && dt == 0);
  energy.push(1000, 3705000, kwh, dt);
  assert(kwh > 0);
  energy.invalidate();
  energy.push(1000, 4000000, kwh, dt);
  assert(kwh == 0);
  energy.push(NAN, 4005000, kwh, dt);
  energy.push(1000, 4010000, kwh, dt);
  assert(kwh == 0);
  // Unsigned monotonic timestamps remain correct across millis rollover.
  EnergyWindow wrap;
  wrap.push(1000, 0xFFFFFF00, kwh, dt);
  wrap.push(1000, 0x000002E8, kwh, dt);
  assert(dt == 1000 && std::abs(kwh - 1.0 / 3600) < 1e-12);
  std::vector<uint8_t> model(203, 0);
  const uint8_t signature[] = {0xE2, 0xD5, 0x0D, 0x08, 0x95, 0x02, 0xD6, 0x10, 0x67, 0x95};
  std::copy(std::begin(signature), std::end(signature), model.begin() + 129);
  assert(PanasonicDecode::getModel(model, 129) == 55);
  std::cout << "Native protocol regression passed\n";
}
