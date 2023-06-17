#pragma once

#ifndef _DRIVERS_HUB238_HPP_
#define _DRIVERS_HUB238_HPP_
#include "configuration.h"
#ifdef POW_PD_EXT
#include <stdbool.h>
#include <stdint.h>

#define HUB238_ADDR 0x08 << 1

#define HUB238_REG_PD_STATUS0  0x00
#define HUB238_REG_PD_STATUS1  0x01
#define HUB238_REG_SRC_PDO_5V  0x02
#define HUB238_REG_SRC_PDO_9V  0x03
#define HUB238_REG_SRC_PDO_12V 0x04
#define HUB238_REG_SRC_PDO_15V 0x05
#define HUB238_REG_SRC_PDO_18V 0x06
#define HUB238_REG_SRC_PDO_20V 0x07
#define HUB238_REG_SRC_PDO     0x08
#define HUB238_REG_GO_COMMAND  0x09

#define HUB238_PDO_DETECTED (0x01 << 7)
// The HUB238 is fairly simple device to interact to, with fairly few registers all in all
//  It only appears to support fixed PDO's up to 20V
//  And they have just dedicated registers to each potential option
//  Given a tip resistance we try and pick the best possible PDO option to suit that resistance
//  (Using I2C overrides any hardware strapping).

// Probe if the hub238 exists on the I2C bus
bool hub238_probe();
// If we have not manually picked a PDO,
// but there is an active PD supply, try for our preference

void hub238_check_negotiation();

// Returns true when negotiation has finished
bool hub238_has_negotiated();
// Returns true when we have run selection and negotiated higher current
bool hub238_has_run_selection();
// Return an encoded state for debugging
uint16_t hub238_debug_state();
// Return selected source voltage in V
uint16_t hub238_source_voltage();
// Return selected source current in Amps * 100
uint8_t hub238_source_currentX100();

uint16_t hub238_getVoltagePDOCurrent(uint8_t voltage);

#endif
#endif
