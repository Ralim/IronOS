#ifndef __OAD_API_H__
#define __OAD_API_H__

#include <stdint.h>

typedef bool (*app_check_oad_cb)(u32_t cur_file_ver, u32_t new_file_ver);
void oad_service_enable(app_check_oad_cb cb);
void ota_finish(struct k_work *work);
#endif //__OAD_API_H__