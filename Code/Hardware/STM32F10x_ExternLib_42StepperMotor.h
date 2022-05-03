#ifndef _STM32F10X_EXTERN_LIB_42_STEPPER_MOTOR_H_
#define _STM32F10X_EXTERN_LIB_42_STEPPER_MOTOR_H_

#include "STM32F10x_ExternLib.h"
#include "rotateDirection.h"

typedef enum StepperMotor42_StatusTypeDef {
    StepperMotor42_Status_Disable, //未在运行的断电状态
    StepperMotor42_Status_Running, //正在运行
    StepperMotor42_Status_Lock     //未在运行，锁死
} StepperMotor42_StatusTypeDef;

typedef struct StepperMotor42_TypeDef {
    struct StepperMotor42_TypeDef *next;
    GPIO_Object GPIO_APositive;          // GPIO A+
    GPIO_Object GPIO_ANegative;          // GPIO A-
    GPIO_Object GPIO_BPositive;          // GPIO B+
    GPIO_Object GPIO_BNegative;          // GPIO B-
    uint32_t stepAngle;                  //步进角，单位0.01°；
    uint32_t easingStep;                 //缓动步数；就是距离开始或结束多少步开始缓动，0为不使用
    RotateDirectionTypeDef direction;    //旋转方向
    uint32_t step;                       //当前在4步的第几步
    uint32_t totalStep;                  //总步数
    uint32_t curStep;                    //当前步数
    uint32_t defaultInterval;            //默认步进时间间隔
    uint32_t curInterval;                //当前步进时间间隔
    uint32_t skipInterval;               //待跳过的间隔
    void (*endCallback)(GenericObject);  //结束回调
    GenericObject endCallbackParam;      //结束回调函数参数
    StepperMotor42_StatusTypeDef status; //该电机是否在工作：

} * StepperMotor42_Object;

typedef struct StepperMotor42Group_typeDef {
    TIM_Object Timer;             // 使用的定时器
    StepperMotor42_Object object; //组中的步进电机对象
} * StepperMotor42Group_Object;

#endif