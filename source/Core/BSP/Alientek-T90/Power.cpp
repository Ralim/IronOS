/*
 * Power.cpp
 *
 *  USB-PD power management for the Alientek T90 (Nations N32L40x).
 *
 *  The T90 sinks power through a CH224Q PD controller driven over a software (bit-bang) I2C
 *  bus on PB6 (SCL) / PB7 (SDA), device address 0x22. We request the configured USB_PD_VMAX
 *  at startup, read back the negotiated voltage, and re-assert the request if the contract
 *  drops. Mirrors the role of Sequre's HUB238/FS2711 external-PD path.
 */

#include "BSP.h"
#include "BSP_Power.h"
#include "Pins.h"
#include "Power.h"
#include "Settings.h"
#include "configuration.h"
#include "n32l40x.h"

// CH224Q register map (from the stock-firmware reverse engineering)
#define CH224Q_ADDR_7BIT  0x22 // 7-bit device address
#define CH224Q_REG_STATUS 0x09 // negotiated-state / capability flags
#define CH224Q_REG_VSEL   0x0A // requested PDO index: 0=5V 1=9V 2=12V 3=15V 4=20V 5=28V

// Voltage-index table matching CH224Q_REG_VSEL. Index -> volts.
static const uint8_t ch224q_voltage_table[] = {5, 9, 12, 15, 20, 28};
#define CH224Q_VSEL_COUNT (sizeof(ch224q_voltage_table) / sizeof(ch224q_voltage_table[0]))

#ifdef CH224_SOFT_I2C

// --- Low-level bit-bang I2C primitives on PB6/PB7 (open-drain emulation) ---

#define SOFT_SCL_HIGH() GPIO_SetBits(SCL2_GPIO_Port, SCL2_Pin)
#define SOFT_SCL_LOW()  GPIO_ResetBits(SCL2_GPIO_Port, SCL2_Pin)
#define SOFT_SDA_HIGH() GPIO_SetBits(SDA2_GPIO_Port, SDA2_Pin)
#define SOFT_SDA_LOW()  GPIO_ResetBits(SDA2_GPIO_Port, SDA2_Pin)
#define SOFT_SDA_READ() (GPIO_ReadInputDataBit(SDA2_GPIO_Port, SDA2_Pin) == Bit_SET ? 1 : 0)

static void soft_i2c_delay(void) {
  // ~100-250 kHz depending on clock; tuned conservatively for the CH224Q.
  for (volatile int i = 0; i < 24; i++) {
    asm("nop");
  }
}

static void soft_i2c_start(void) {
  SOFT_SDA_HIGH();
  SOFT_SCL_HIGH();
  soft_i2c_delay();
  SOFT_SDA_LOW();
  soft_i2c_delay();
  SOFT_SCL_LOW();
  soft_i2c_delay();
}

static void soft_i2c_stop(void) {
  SOFT_SDA_LOW();
  soft_i2c_delay();
  SOFT_SCL_HIGH();
  soft_i2c_delay();
  SOFT_SDA_HIGH();
  soft_i2c_delay();
}

// Returns true if the slave ACKed (SDA pulled low on the 9th clock).
static bool soft_i2c_write_byte(uint8_t byte) {
  for (uint8_t bit = 0; bit < 8; bit++) {
    if (byte & 0x80) {
      SOFT_SDA_HIGH();
    } else {
      SOFT_SDA_LOW();
    }
    byte <<= 1;
    soft_i2c_delay();
    SOFT_SCL_HIGH();
    soft_i2c_delay();
    SOFT_SCL_LOW();
    soft_i2c_delay();
  }
  // Release SDA and clock in the ACK bit.
  SOFT_SDA_HIGH();
  soft_i2c_delay();
  SOFT_SCL_HIGH();
  soft_i2c_delay();
  bool ack = (SOFT_SDA_READ() == 0);
  SOFT_SCL_LOW();
  soft_i2c_delay();
  return ack;
}

static uint8_t soft_i2c_read_byte(bool ack) {
  uint8_t byte = 0;
  SOFT_SDA_HIGH(); // release for the slave to drive
  for (uint8_t bit = 0; bit < 8; bit++) {
    byte <<= 1;
    soft_i2c_delay();
    SOFT_SCL_HIGH();
    soft_i2c_delay();
    if (SOFT_SDA_READ()) {
      byte |= 1;
    }
    SOFT_SCL_LOW();
    soft_i2c_delay();
  }
  // Master ACK/NACK
  if (ack) {
    SOFT_SDA_LOW();
  } else {
    SOFT_SDA_HIGH();
  }
  soft_i2c_delay();
  SOFT_SCL_HIGH();
  soft_i2c_delay();
  SOFT_SCL_LOW();
  soft_i2c_delay();
  SOFT_SDA_HIGH();
  return byte;
}

static bool ch224q_write_reg(uint8_t reg, uint8_t val) {
  soft_i2c_start();
  if (!soft_i2c_write_byte((CH224Q_ADDR_7BIT << 1) | 0)) {
    soft_i2c_stop();
    return false;
  }
  if (!soft_i2c_write_byte(reg)) {
    soft_i2c_stop();
    return false;
  }
  bool ok = soft_i2c_write_byte(val);
  soft_i2c_stop();
  return ok;
}

static bool ch224q_read_reg(uint8_t reg, uint8_t *out) {
  soft_i2c_start();
  if (!soft_i2c_write_byte((CH224Q_ADDR_7BIT << 1) | 0)) {
    soft_i2c_stop();
    return false;
  }
  if (!soft_i2c_write_byte(reg)) {
    soft_i2c_stop();
    return false;
  }
  soft_i2c_start(); // repeated start
  if (!soft_i2c_write_byte((CH224Q_ADDR_7BIT << 1) | 1)) {
    soft_i2c_stop();
    return false;
  }
  *out = soft_i2c_read_byte(false); // single byte -> NACK
  soft_i2c_stop();
  return true;
}

#endif // CH224_SOFT_I2C

// Highest voltage index we will request (clamped to USB_PD_VMAX).
static uint8_t requested_index = 0;
// Last voltage we believe is negotiated, in volts.
static uint8_t negotiated_voltage = 5;

static uint8_t voltage_to_index(uint8_t volts) {
  uint8_t best = 0;
  for (uint8_t i = 0; i < CH224Q_VSEL_COUNT; i++) {
    if (ch224q_voltage_table[i] <= volts) {
      best = i;
    }
  }
  return best;
}

void ch224q_init(void) {
#ifdef CH224_SOFT_I2C
  // Bus pins are configured as open-drain outputs by Setup.cpp; idle both lines high.
  SOFT_SCL_HIGH();
  SOFT_SDA_HIGH();
  requested_index = voltage_to_index(USB_PD_VMAX);
  ch224q_write_reg(CH224Q_REG_VSEL, requested_index);
  negotiated_voltage = ch224q_voltage_table[requested_index];
#endif
}

// Negotiated source voltage in volts (used by preStartChecks for the PWM-speed decision).
uint16_t ch224q_source_voltage(void) { return negotiated_voltage; }

// Source current capability x100 (in centi-amps). The CH224Q does not expose a precise
// current value over this register set; assume the CH224Q maximum 5A contract. Conservative.
uint16_t ch224q_source_currentX100(void) { return 500; }

void power_check() {
#if defined(POW_PD_EXT) && (POW_PD_EXT == 3)
#ifdef CH224_SOFT_I2C
  // Re-assert the desired PDO if the negotiated voltage looks wrong (contract dropped).
  uint8_t status = 0;
  if (ch224q_read_reg(CH224Q_REG_STATUS, &status)) {
    // A zeroed status register indicates no live contract; re-request our voltage.
    if (status == 0) {
      ch224q_write_reg(CH224Q_REG_VSEL, requested_index);
    }
  } else {
    // Bus error: re-assert blindly.
    ch224q_write_reg(CH224Q_REG_VSEL, requested_index);
  }
  negotiated_voltage = ch224q_voltage_table[requested_index];
#endif
#endif
}

bool getIsPoweredByDCIN() { return false; }
