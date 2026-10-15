#include <cassert>
#include <cstdio>

// Exercise the actual shared controller, including its private timing state.
#include "PIDThread.cpp"

static TickType_t testTicks;
static uint32_t   availablePower = 2000;
TickType_t        xTaskGetTickCount() { return testTicks; }
uint32_t          availableW10(uint8_t) { return availablePower; }
uint16_t          getSettingValue(SettingsOptions option) { return option == SettingsOptions::PowerLimit ? 100 : 0; }
uint16_t          getTipThermalMass() { return TIP_THERMAL_MASS; }
uint16_t          getTipInertia() { return TIP_THERMAL_INERTIA; }

int main() {
  static_assert(TIP_THERMAL_MASS == 10 && TIP_THERMAL_INERTIA == 128, "Use the SEQURE S60/S60P C210 tuning");
  static_assert(SOLDERING_TEMP == 320 && MAX_TEMP_C == 450 && MAX_TEMP_F == 850, "Use normal IronOS temperature settings");
  static_assert(HARDWARE_MAX_WATTAGE_X10 == 1000, "Keep the requested 100 W ceiling");
  testTicks = 5000;
  assert(getPIDResultX10Watts(450, 25) == 1000);
  assert(getPIDResultX10Watts(450, 25) == 1000);
  testTicks += 20000;
  assert(getPIDResultX10Watts(320, 25) == 1000);
  testTicks = UINT32_MAX - 10;
  assert(getPIDResultX10Watts(320, 25) == 1000);
  testTicks = 20;
  assert(getPIDResultX10Watts(320, 25) == 1000);
  for (unsigned i = 0; i < 100; ++i) {
    testTicks += 125;
    const int32_t output = getPIDResultX10Watts(320, 500);
    assert(output >= -1000 && output <= 1000);
    if (i > 1) {
      assert(output <= 0);
    }
  }
  availablePower = 125;
  testTicks += 125;
  assert(getPIDResultX10Watts(450, 25) == 125);
  availablePower       = 2000;
  int32_t holdingPower = 0;
  for (unsigned i = 0; i < 128; ++i) {
    testTicks += 125;
    holdingPower = getPIDResultX10Watts(320, 315);
  }
  assert(holdingPower > 0 && holdingPower < HARDWARE_MAX_WATTAGE_X10);
  testTicks += 125;
  assert(getPIDResultX10Watts(450, 25) == 1000);
  testTicks += 125;
  assert(getPIDResultX10Watts(450, 499) > 0);
  requestPIDReset(); // A recovered fault must not carry stored heat into a hot restart.
  testTicks += 125;
  assert(getPIDResultX10Watts(450, 499) <= 0);
  requestPIDReset();
  testTicks += 125;
  assert(getPIDResultX10Watts(450, 25) == 1000);
  puts("TS21 C210 controller tests passed: shared tuning, idle/same-tick/rollover timing and bounded output");
}
