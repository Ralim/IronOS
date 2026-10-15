#include "Settings.h"
#include "Translation.h"
#include <cassert>
#include <cstdio>
#include <cstring>

static uint8_t  flashData[256];
static unsigned flashWrites;
const bool      HasFahrenheit = true;

extern "C" void flash_read_buffer(uint8_t *buffer, uint16_t length) {
  assert(length <= sizeof(flashData));
  std::memcpy(buffer, flashData, length);
}

extern "C" void flash_save_buffer(const uint8_t *buffer, uint16_t length) {
  assert(length <= sizeof(flashData));
  std::memcpy(flashData, buffer, length);
  ++flashWrites;
}

int main() {
  static_assert(SettingsOptions::WorklightEnabled == 57 && SettingsOptions::SettingsOptionsLength == 58, "Append the work-light setting without moving existing values");
  static_assert(sizeof(systemSettingsType) <= sizeof(flashData), "Keep the TS21 settings page bounds");
  systemSettingsType legacy;
  std::memset(&legacy, 0xFF, sizeof(legacy));
  legacy.versionMarker = SETTINGSVERSION;
  legacy.length        = 57;
  for (unsigned i = 0; i < legacy.length; ++i) {
    legacy.settingsValues[i] = settingsConstants[i].defaultValue;
  }
  legacy.settingsValues[SettingsOptions::SolderingTemp]    = 350;
  legacy.settingsValues[SettingsOptions::PowerLimit]       = 60;
  legacy.settingsValues[SettingsOptions::WorklightEnabled] = 1; // Old alignment padding must not enable the new option.
  std::memset(flashData, 0xFF, sizeof(flashData));
  std::memcpy(flashData, &legacy, sizeof(legacy));

  assert(loadSettings());
  assert(flashWrites == 1);
  assert(getSettingValue(SettingsOptions::WorklightEnabled) == 0);
  for (unsigned i = 0; i < legacy.length; ++i) {
    assert(getSettingValue(static_cast<SettingsOptions>(i)) == legacy.settingsValues[i]);
  }
  systemSettingsType saved;
  std::memcpy(&saved, flashData, sizeof(saved));
  assert(saved.length == 58);

  setSettingValue(SettingsOptions::WorklightEnabled, 1);
  saveSettings();
  assert(flashWrites == 2);
  setSettingValue(SettingsOptions::WorklightEnabled, 0);
  assert(!loadSettings());
  assert(getSettingValue(SettingsOptions::WorklightEnabled) == 1);
  nextSettingValue(SettingsOptions::WorklightEnabled);
  assert(getSettingValue(SettingsOptions::WorklightEnabled) == 0);
  saveSettings();
  setSettingValue(SettingsOptions::WorklightEnabled, 1);
  assert(!loadSettings());
  assert(getSettingValue(SettingsOptions::WorklightEnabled) == 0);
  assert(getSettingValue(SettingsOptions::SolderingTemp) == 350);
  assert(getSettingValue(SettingsOptions::PowerLimit) == 60);
  puts("TS21 settings tests passed: legacy values preserved, work light defaults off and persists on/off");
}
