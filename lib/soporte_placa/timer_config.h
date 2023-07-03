#ifndef TIMER_CONFIG_H
#define TIMER_CONFIG_H

#include "stm32f1xx.h"

//void configurar_TIM2(uint32_t duty_cycle);
void TIM2_init(void);
void configurar_TIM2(int angulo);
int32_t obtener_angulo(void);

#endif