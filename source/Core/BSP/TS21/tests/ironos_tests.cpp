#include "Font.h"
#include "OLED.hpp"
#include "OperatingModes.h"
#include "Settings.h"
#include "power.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <vector>

const uint16_t    powerPWM = 255;
uint16_t          totalPWM = 255;
static unsigned   calls;
static uint8_t    buttons;
static TickType_t ticks;
static uint16_t   inputVoltage = 50;
#ifdef TS21_HEATER_TEST
static uint8_t             actualPWM;
volatile struct TS21Status ts21_status;
#endif

void setTipPWM(uint8_t pulse, bool) {
#ifdef TS21_HEATER_TEST
  actualPWM = std::min<uint8_t>(pulse, 7);
#else
  assert(pulse == 0);
#endif
  ++calls;
}
#ifdef TS21_HEATER_TEST
uint32_t getTipX10Watts() { return actualPWM * availableW10(0) / powerPWM; }
#endif
uint16_t getInputVoltageX10(uint16_t, uint8_t) { return inputVoltage; }
uint8_t  getTipResistanceX10() { return 20; }
uint16_t getSettingValue(SettingsOptions) { return VOLTAGE_DIV; }

void       vTaskDelay(TickType_t) {}
TickType_t xTaskGetTickCount() { return ticks; }
uint8_t    getButtonA() { return (buttons & 1) != 0; }
uint8_t    getButtonB() { return (buttons & 2) != 0; }
bool       isTipDisconnected() { return false; }

OperatingMode handleHomeButtons(ButtonState state, guiContext *context);

static void testButtonActions() {
  static_assert(configTICK_RATE_HZ == 1000, "Keep menu and long-press timing in milliseconds");
  assert(getButtonState() == BUTTON_NONE);
  guiContext context{};
  assert(handleHomeButtons(BUTTON_NONE, &context) == OperatingMode::HomeScreen);
  buttons = 2;
  assert(getButtonState() == BUTTON_NONE);
  ticks += 120;
  assert(getButtonState() == BUTTON_NONE);
  buttons               = 0;
  const ButtonState top = getButtonState();
  assert(top == BUTTON_B_SHORT);
  assert(handleHomeButtons(top, &context) == OperatingMode::SettingsMenu);
  assert(getButtonState() == BUTTON_NONE);
  buttons = 1;
  assert(getButtonState() == BUTTON_NONE);
  ticks += 120;
  buttons                  = 0;
  const ButtonState bottom = getButtonState();
  assert(bottom == BUTTON_F_SHORT);
  assert(handleHomeButtons(bottom, &context) == OperatingMode::Soldering);
#ifdef TS21_HEATER_TEST
  ts21_status.errors = TS21_ERROR_HEATER_SHORT;
  assert(handleHomeButtons(bottom, &context) == OperatingMode::ThermalRunaway);
  assert(handleHomeButtons(top, &context) == OperatingMode::SettingsMenu);
  ts21_status.errors = TS21_ERROR_UART;
  assert(handleHomeButtons(bottom, &context) == OperatingMode::Soldering);
  ts21_status.errors = 0;
#endif
  buttons = 2;
  assert(getButtonState() == BUTTON_NONE);
  ticks += 399;
  assert(getButtonState() == BUTTON_NONE);
  ++ticks;
  assert(getButtonState() == BUTTON_B_LONG);
  buttons = 0;
  assert(getButtonState() == BUTTON_NONE);
  buttons = 3;
  assert(getButtonState() == BUTTON_NONE);
  ticks += 120;
  buttons = 0;
  assert(getButtonState() == BUTTON_BOTH);
  assert(getButtonState() == BUTTON_NONE);
}

static std::vector<uint8_t>                           oledCommands;
static std::array<std::array<uint8_t, OLED_WIDTH>, 4> oledPages;
static std::array<unsigned, 4>                        oledPageWrites;

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t control, uint8_t value) {
  assert(address == DEVICEADDR_OLED && control == 0);
  oledCommands.push_back(value);
  return true;
}

bool FRToSI2C::Mem_Write(uint16_t address, uint16_t control, uint8_t *data, uint16_t size) {
  assert(address == DEVICEADDR_OLED && control == 0x40 && size == OLED_WIDTH);
  assert(oledCommands.size() >= 3);
  const size_t start = oledCommands.size() - 3;
  assert(oledCommands[start] >= 0xB0 && oledCommands[start] <= 0xB3);
  assert(oledCommands[start + 1] == 0x10 && oledCommands[start + 2] == 0);
  const unsigned page = oledCommands[start] - 0xB0;
  std::copy_n(data, size, oledPages[page].begin());
  ++oledPageWrites[page];
  return true;
}

static void testOLEDMapping() {
  static_assert(OLED_WIDTH == 128 && OLED_HEIGHT == 32, "Keep the complete TS21 interface");
  OLED::initialize();
  const std::array<uint8_t, 5> initialMapping = {0xC0, 0xD3, 0x30, 0x40, 0xA0};
  assert(std::search(oledCommands.begin(), oledCommands.end(), initialMapping.begin(), initialMapping.end()) != oledCommands.end());
  const std::array<uint8_t, 2> sequentialCOM = {0xDA, 0x02};
  assert(std::search(oledCommands.begin(), oledCommands.end(), sequentialCOM.begin(), sequentialCOM.end()) != oledCommands.end());
  const std::array<uint8_t, 2> pageAddressing = {0x20, 0x02};
  assert(std::search(oledCommands.begin(), oledCommands.end(), pageAddressing.begin(), pageAddressing.end()) != oledCommands.end());

  std::array<uint8_t, OLED_WIDTH * 4> frame;
  for (unsigned row = 0; row < 4; ++row) {
    for (unsigned x = 0; x < OLED_WIDTH; ++x) {
      frame[row * OLED_WIDTH + x] = static_cast<uint8_t>((row + 1) * 31 + x);
    }
  }
  OLED::drawArea(0, 0, OLED_WIDTH, OLED_HEIGHT, frame.data());
  for (unsigned pass = 0; pass < 3; ++pass) {
    oledCommands.clear();
    oledPageWrites.fill(0);
    if (pass == 0) {
      OLED::setRotation(false);
      assert(oledCommands.empty());
      OLED::refresh();
    } else {
      const bool left = pass == 1;
      OLED::setRotation(left);
      const std::array<uint8_t, 4> mapping = {static_cast<uint8_t>(left ? 0xC8 : 0xC0), 0xD3, static_cast<uint8_t>(left ? 0x10 : 0x30), static_cast<uint8_t>(left ? 0xA1 : 0xA0)};
      assert(oledCommands.size() >= mapping.size());
      assert(std::equal(mapping.begin(), mapping.end(), oledCommands.begin()));
      assert(OLED::getRotation() == left);
    }
    for (unsigned row = 0; row < 4; ++row) {
      assert(oledPageWrites[row] == 1);
      assert(std::equal(oledPages[row].begin(), oledPages[row].end(), frame.begin() + row * OLED_WIDTH));
    }
  }
}

void           ui_pre_render_assets(void);
extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

int main() {
  for (int32_t watts = -100; watts <= 2000; ++watts) {
    setTipX10Watts(watts);
#ifndef TS21_HEATER_TEST
    assert(x10WattHistory.average() == 0);
#endif
  }
  setTipX10Watts(INT32_MAX);
  setTipX10Watts(INT32_MIN);
  assert(calls == 2103);
#ifndef TS21_HEATER_TEST
  assert(x10WattHistory.average() == 0);
#else
  x10WattHistory.sum = 3;
  for (unsigned i = 0; i < 100; ++i) {
    setTipX10Watts(100);
  }
  assert(actualPWM == 7 && x10WattHistory.average() == 3);
  assert(X10WattsToPWM(INT32_MAX, 0) == 255);
  assert(X10WattsToPWM(INT32_MIN, 0) == 0);
  inputVoltage = 0;
  assert(X10WattsToPWM(1, 0) == 0);
  setTipX10Watts(100);
  assert(actualPWM == 0);
  inputVoltage = 50;
#endif

  ui_pre_render_assets();
  for (unsigned row = 0; row < 4; ++row) {
    for (unsigned x = 0; x < 56; ++x) {
      const unsigned source = row * 56 + 55 - x, destination = row * 56 + x;
      assert(buttonAF[destination] == buttonA[source]);
      assert(buttonBF[destination] == buttonB[source]);
      assert(disconnectedTipF[destination] == disconnectedTip[source]);
    }
  }
  testOLEDMapping();
  testButtonActions();
  puts("TS21 IronOS tests passed: heater lockout, display mapping, complete 128x32 frames, menu button actions and press timing");
}
