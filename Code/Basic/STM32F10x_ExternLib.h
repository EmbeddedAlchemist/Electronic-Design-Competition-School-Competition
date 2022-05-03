#ifndef _STM32F10x_EXTERN_LIB_H_
#define _STM32F10x_EXTERN_LIB_H_

typedef void *GenericObject;//通用对象

//标准库
#include "stdio.h"
#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"

//标准固件库
#include "stm32f10x.h"

//扩展库
#include "STM32F10x_ExternLib_GPIO.h"
#include "STM32F10x_ExternLib_PWM.h"
#include "STM32F10x_ExternLib_Timer.h"
#include "MemoryManage.h"

const GenericObject EmptyObject = NULL;

#endif
