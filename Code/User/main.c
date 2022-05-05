/*
- 工程模板
- 创建于2022/01/09
- 基于STM32F1XX固件库V3.5.0
- 记得改芯片型号和启动文件
*/

#include "STM32F10x_ExternLib.h"

#include "STM32F10x_ExternLib_42StepperMotor.h"

StepperMotor42Group_Object motorGroup;
StepperMotor42_Object motor;


int main(){
    STM32F10x_ExternLib_Initialize();
    motorGroup = StepperMotor42_InitializeGroup(TIM_GenerateUninitialize(TIM2, TIM2_IRQn));
    motor = StepperMotor42_Initialize(GPIO_GenerateUninitialize(GPIOC, GPIO_Pin_3),
                                      GPIO_GenerateUninitialize(GPIOC, GPIO_Pin_0),
                                      GPIO_GenerateUninitialize(GPIOC, GPIO_Pin_1),
                                      GPIO_GenerateUninitialize(GPIOC, GPIO_Pin_2),
                                      1.8f,
                                      0);
    StepperMotor42_AddToGroup(motorGroup, motor);
    StepperMotor42_RotateCycle(motor, RotateDirection_Clockwise, 1.0f);
    for(;;){
    }
}
