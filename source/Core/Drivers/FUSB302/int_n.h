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

#ifndef PDB_INT_N_OLD_H
#define PDB_INT_N_OLD_H

#include <pd.h>

class InterruptHandler {
public:
  // Creates the thread to handle the Interrupt pin
  static void init();

  static void irqCallback();

private:
  static void                Thread(const void *arg);
  static volatile osThreadId TaskHandle;
  static const size_t        TaskStackSize = 1536 / 3;
  static uint32_t            TaskBuffer[TaskStackSize];
  static osStaticThreadDef_t TaskControlBlock;
  /*
   * Hard Reset machine states
   */
  enum hardrst_state { PRLHRResetLayer, PRLHRIndicateHardReset, PRLHRRequestHardReset, PRLHRWaitPHY, PRLHRHardResetRequested, PRLHRWaitPE, PRLHRComplete };
  static enum hardrst_state hardrst_reset_layer();
  static enum hardrst_state hardrst_indicate_hard_reset();
  static enum hardrst_state hardrst_request_hard_reset();
  static enum hardrst_state hardrst_wait_phy();
  static enum hardrst_state hardrst_hard_reset_requested();
  static enum hardrst_state hardrst_wait_pe();
  static enum hardrst_state hardrst_complete();
  // Mesage rx
  static void         readPendingMessage();
  static union pd_msg tempMessage;
};

#endif /* PDB_INT_N_OLD_H */
