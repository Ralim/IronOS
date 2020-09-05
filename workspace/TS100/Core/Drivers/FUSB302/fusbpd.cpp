/*
 * fusbpd.cpp
 *
 *  Created on: 13 Jun 2020
 *      Author: Ralim
 */
#include "Model_Config.h"
#ifdef POW_PD
#include <fusbpd.h>
#include <pd.h>
#include "BSP.h"
#include "I2CBB.hpp"
#include "fusb302b.h"
#include "policy_engine.h"
#include "protocol_rx.h"
#include "protocol_tx.h"
#include "int_n.h"
#include "hard_reset.h"

void fusb302_start_processing() {
	/* Initialize the FUSB302B */
	fusb_setup();
	ResetHandler::init();
	PolicyEngine::init();
	ProtocolTransmit::init();
	ProtocolReceive::init();
	InterruptHandler::init();
}
#endif
