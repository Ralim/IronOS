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

#ifndef PDB_HARD_RESET_H
#define PDB_HARD_RESET_H

#include <pd.h>

/* Events for the Hard Reset thread */
#define PDB_EVT_HARDRST_RESET EVENT_MASK(0)
#define PDB_EVT_HARDRST_I_HARDRST EVENT_MASK(1)
#define PDB_EVT_HARDRST_I_HARDSENT EVENT_MASK(2)
#define PDB_EVT_HARDRST_DONE EVENT_MASK(3)

class ResetHandler {
public:
	static void init();
	static void notify(uint32_t notification);
private:
	static void Thread(const void *arg);
	static osThreadId TaskHandle;
	static const size_t TaskStackSize = 1536 / 2;
	static uint32_t TaskBuffer[TaskStackSize];
	static osStaticThreadDef_t TaskControlBlock;
	static uint32_t waitForEvent(uint32_t mask, uint32_t ticksToWait =
			portMAX_DELAY);

	/*
	 * Hard Reset machine states
	 */
	enum hardrst_state {
		PRLHRResetLayer,
		PRLHRIndicateHardReset,
		PRLHRRequestHardReset,
		PRLHRWaitPHY,
		PRLHRHardResetRequested,
		PRLHRWaitPE,
		PRLHRComplete
	};
	static hardrst_state hardrst_reset_layer();
	static hardrst_state hardrst_indicate_hard_reset();
	static hardrst_state hardrst_request_hard_reset();
	static hardrst_state hardrst_wait_phy();
	static hardrst_state hardrst_hard_reset_requested();
	static hardrst_state hardrst_wait_pe();
	static hardrst_state hardrst_complete();
};

#endif /* PDB_HARD_RESET_H */
