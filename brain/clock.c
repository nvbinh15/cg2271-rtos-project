#include "MKL25Z4.h"
#include "clock.h"

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "os_tick.h"

void kernelDelayUs(uint32_t time) {
  uint32_t currTick = osKernelGetSysTimerCount();
  uint32_t waitTicks = time * osKernelGetSysTimerFreq() / 1000000;
  
  while (osKernelGetSysTimerCount() - currTick < waitTicks);
}
