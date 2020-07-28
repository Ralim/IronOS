#include "BSP.h"
#include "FreeRTOS.h"
#include "QC3.h"
#include "Settings.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "task.h"
#include "I2C_Wrapper.hpp"
#include "fusbpd.h"
void postRToSInit() {
	// Any after RTos setup
	FRToSI2C::FRToSInit();
#ifdef POW_PD
	//Spawn all of the USB-C processors
	fusb302_start_processing();
#endif
}
