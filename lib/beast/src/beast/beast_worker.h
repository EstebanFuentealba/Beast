#include "beast.h"

#ifndef __BEAST_WORKER_H__
#define __BEAST_WORKER_H__

void beast_worker_input_task(void *pvParameters);
void beast_worker_save_firmware_state(size_t app);
size_t beast_worker_load_firmware_state();
void beast_worker_switch_firmware(size_t app);

#endif