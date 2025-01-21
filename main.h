#ifndef __main__
#define __main__

// khai bao cac thu vien can thiet
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_flash.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_exti.h>
#include "OSkernel.h"

//khai bao thoi gian cua 1 task
#define QUANTA 10

//khai bao nguyen mau ham cac ham thuc thi
void do1_xanh2 (void);
void do1_vang2 (void);
void xanh1_do2 (void);
void vang1_do2 (void);
void reset_GPIO(void);
void task1(void);
void task2(void);
void sporadictask(void);
void RCC_Init(void);
void GPIO_Config(void);
void TIMER2_Init(void);
int main(void);

#endif
