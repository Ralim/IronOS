/*
 * PD Buddy Firmware Library - USB Power Delivery for everyone
 * Copyright 2017-2018 Clayton G. Hobbs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PDB_PROTOCOL_RX_H
#define PDB_PROTOCOL_RX_H

#include <stdint.h>

#include <pd.h>

/* Events for the Protocol RX thread */
#define PDB_EVT_PRLRX_RESET EVENT_MASK(0)
#define PDB_EVT_PRLRX_I_GCRCSENT EVENT_MASK(1)

class ProtocolReceive {
public:
	static void init();
	static void notify(uint32_t notification);
private:
	static void thread(const void *args);

	static EventGroupHandle_t xEventGroupHandle;
	static StaticEventGroup_t xCreatedEventGroup;
	static osThreadId TaskHandle;
	static const size_t TaskStackSize = 1024 / 4;
	static uint32_t TaskBuffer[TaskStackSize];
	static osStaticThreadDef_t TaskControlBlock;
	/*
	 * Protocol RX machine states
	 *
	 * There is no Send_GoodCRC state because the PHY sends the GoodCRC for us.
	 * All transitions that would go to that state instead go to Check_MessageID.
	 */
	enum protocol_rx_state {
		PRLRxWaitPHY, PRLRxReset, PRLRxCheckMessageID, PRLRxStoreMessageID
	};
	static protocol_rx_state protocol_rx_store_messageid();
	static protocol_rx_state protocol_rx_check_messageid();
	static protocol_rx_state protocol_rx_reset();
	static protocol_rx_state protocol_rx_wait_phy();
	static union pd_msg tempMessage;
	static uint8_t _rx_messageid;
	static uint8_t _tx_messageidcounter;
	static uint32_t waitForEvent(uint32_t mask, uint32_t ticksToWait =
	portMAX_DELAY);

};

#endif /* PDB_PROTOCOL_RX_H */
