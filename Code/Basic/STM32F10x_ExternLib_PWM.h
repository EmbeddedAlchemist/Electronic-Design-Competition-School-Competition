#ifndef _STM32F10X_EXTERN_LIB_PWM_H_
#define _STM32F10X_EXTERN_LIB_PWM_H_

#include "STM32F10x_ExternLib_Timer.h"

typedef struct PWM_TypeDef{
    TIM_PeriphTypeDef *TIM_Periph;//PWM所使用的定时器
    uint16_t TIM_Channel;//在指定计时器的第几个通道
    uint16_t duty;
} *PWM_Object;
#endif
