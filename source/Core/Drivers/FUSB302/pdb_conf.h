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

#ifndef PDB_CONF_H
#define PDB_CONF_H

/* Number of messages in the message pool */
#define PDB_MSG_POOL_SIZE 4

#define EVENT_MASK(x) (1 << x)
#define eventmask_t   uint32_t
/* PD Buddy thread priorities */
#define PDB_PRIO_PE        (osPriorityNormal)
#define PDB_PRIO_PRL       (osPriorityBelowNormal)
#define PDB_PRIO_PRL_INT_N (osPriorityLow)

#endif /* PDB_CONF_H */
