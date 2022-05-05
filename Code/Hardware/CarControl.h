#ifndef _CAR_CONTROL_H_
#define _CAR_CONTROL_H_

#include "STM32F10x_ExternLib.h"
#include "STM32F10x_ExternLib_42StepperMotor.h"

typedef enum Car_TaskTypeTypeDef { //小车所有的动作都可以分解为旋转和直行两种...
    Car_TaskType_Rotate,           //旋转
    Car_TaskType_Move              //直行
} Car_TaskTypeTypeDef;

typedef enum Car_TaskStatusTypeDef {
    Car_TaskStatus_Done,//任务执行完毕
    Car_TaskStatus_Running,//正在执行
    Car_TaskStatus_Waiting,//在队列中等待执行
    Car_TaskStatus_Error//错误
} Car_TaskStatusTypeDef;

typedef struct Car_TaskTypeDef{
    struct Car_TaskTypeDef *next;//下一条任务...
    uint32_t index;//为每个任务分配的唯一序列号，可通过这个序列号查询任务执行状态
    Car_TaskTypeTypeDef type;//任务类型
    int32_t param;//任务参数
    bool leftWheelTaskEnd;//左轮任务结束
    bool rightWheelTaskEnd;//右轮任务结束
} * Car_TaskObject;

typedef struct Car_TypeDef{
    StepperMotor42_Object leftWheel;//左轮
    StepperMotor42_Object rightWheel;//右轮
    uint32_t wheelRadius;//轮半径，单位mm
    uint32_t wheelHalfDistance;//两轮之间的一半距离
    int32_t locateX;//小车在场地的X坐标
    int32_t locateY;//小车在场地的Y坐标
    uint32_t locateAngel;//小车在场地的角度，单位°，以x轴方向为0°，范围0°到360°
    uint32_t lastTaskIndex;
    Car_TaskObject taskSequence;
} * Car_Object;

#endif