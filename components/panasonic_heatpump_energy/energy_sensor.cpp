#include "energy_sensor.h"
#include <cmath>
namespace esphome {
namespace panasonic_heatpump_energy {
void DailyEnergy::setup() {
  if (!power_->is_extra_power_of(heatpump_)) {
    ESP_LOGE("panasonic_heatpump_energy", "power_id must reference XTOP0..5 from heatpump_id");
    mark_failed();
    return;
  }
  pref_ = make_entity_preference<Saved>();
  loaded_ =
      restore_ && pref_.load(&saved_) && saved_.version == 1 && std::isfinite(saved_.energy) && saved_.energy >= 0;
  power_->add_on_raw_state_callback([this](float watts) { sample_(watts); });
}
bool DailyEnergy::initialize_(const ESPTime& now) {
  if (!now.is_valid()) {
    window_.invalidate();
    have_epoch_ = false;
    return false;
  }
  if (!initialized_) {
    date_value_ = date_(now);
    today_ = loaded_ ? (saved_.date == date_value_ ? saved_.energy : 0) : initial_energy_;
    initialized_ = true;
    persist_();
  }
  return true;
}
void DailyEnergy::persist_() {
  publish_state(float(today_));
  if (restore_) {
    saved_.version = 1;
    saved_.date = date_value_;
    saved_.energy = today_;
    pref_.save(&saved_);  // buffered by ESPHome preferences; no forced flash sync
  }
}
void DailyEnergy::loop() {
  if (is_failed())
    return;
  const uint32_t ms = millis();
  if (!panasonic_heatpump::protocol::elapsed(ms, last_loop_, 1000))
    return;
  last_loop_ = ms;
  const auto now = clock_->now();
  if (!initialize_(now))
    return;
  if (!heatpump_->extra_fresh() ||
      (have_epoch_ && panasonic_heatpump::protocol::elapsed(ms, last_sample_ms_, window_.max_gap))) {
    window_.invalidate();
    have_epoch_ = false;
    // No valid sample can straddle this day boundary: reset even at idle.
    if (date_(now) != date_value_) {
      today_ = 0;
      date_value_ = date_(now);
      persist_();
    }
  }
}
void DailyEnergy::sample_(float watts) {
  const auto now = clock_->now();
  if (!initialize_(now))
    return;
  if (!heatpump_->extra_fresh() || !std::isfinite(watts) || watts < 0) {
    window_.invalidate();
    have_epoch_ = false;
    return;
  }
  const uint32_t generation = heatpump_->get_extra_generation();
  if (generation == last_generation_)
    return;  // callbacks cannot replay cached data
  last_generation_ = generation;
  const uint32_t ms = heatpump_->get_last_extra_time();
  const uint32_t duration = ms - last_sample_ms_;
  // Reject large NTP jumps, including time moving backwards, rather than
  // assigning an uncertain interval to an invented calendar date.
  if (have_epoch_ && std::abs(double(int64_t(now.timestamp) - last_epoch_) * 1000 - duration) > 2000)
    window_.invalidate();
  double kwh;
  uint32_t integrated_ms;
  window_.push(watts, ms, kwh, integrated_ms);
  const uint32_t new_date = date_(now);
  if (new_date != date_value_) {
    auto midnight = now;
    midnight.hour = 0;
    midnight.minute = 0;
    midnight.second = 0;
    midnight.recalc_timestamp_local();
    const uint32_t today_ms = uint32_t(now.timestamp - midnight.timestamp) * 1000;
    const auto split = panasonic_heatpump::protocol::split_at_midnight(kwh, integrated_ms, today_ms);
    // Only a small contiguous interval may cross a date boundary.
    // Publish the final old-day total before reset, then today's fraction.
    if (integrated_ms && today_ms < integrated_ms) {
      today_ += split.previous;
      persist_();
    }
    today_ = split.current;
    date_value_ = new_date;
    persist_();
  } else {
    today_ += kwh;
    persist_();
  }
  last_epoch_ = now.timestamp;
  last_sample_ms_ = ms;
  have_epoch_ = true;
}
}  // namespace panasonic_heatpump_energy
}  // namespace esphome
