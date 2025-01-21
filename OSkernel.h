#ifndef __OSkernel__
#define __OSkernel__

#include <stm32f10x.h>

void osKernelInit(void);
void osKernelLaunch(uint32_t quanta);
void osSchedulerLaunch(void);
void osKernelStackInit(int i);
uint8_t osKernelAdd1Thread(void (*task)(void));
void osSchedulerRoundRobin(void);
void osThreadYield(void);
void osEdgeTriggerInit(uint32_t *semaphore);
void EdgeTrigger_Init(void);
void osSemaphore_Init(uint32_t *semaphore, uint32_t val);
void osSemaphore_Give(uint32_t *semaphore);
void osCooperative_Wait(uint32_t *semaphore);

#endif
