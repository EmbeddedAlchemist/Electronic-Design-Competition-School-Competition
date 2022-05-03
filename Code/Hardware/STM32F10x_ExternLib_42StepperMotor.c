#include "STM32F10x_ExternLib_42StepperMotor.h"

StepperMotor42_Object StepperMotor42_Initialize(GPIO_Object GPIO_APositive,
                                                GPIO_Object GPIO_ANegative,
                                                GPIO_Object GPIO_BPositive,
                                                GPIO_Object GPIO_BNagitive,
                                                float stepAngle,
                                                uint32_t easing) {
    GPIO_InitializeObject(GPIO_APositive, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    GPIO_InitializeObject(GPIO_ANegative, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    GPIO_InitializeObject(GPIO_BPositive, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    GPIO_InitializeObject(GPIO_BNagitive, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    StepperMotor42_Object newStepperMotor42Object = (StepperMotor42_Object)MeM_Request(sizeof(struct StepperMotor42_TypeDef)); //新建对象
    if (!newStepperMotor42Object)
        return NULL;
    newStepperMotor42Object->stepAngle = (uint32_t)(stepAngle / 2 * 100);
    newStepperMotor42Object->easingStep = easing;
    newStepperMotor42Object->next = EmptyObject;
}

StepperMotor42Group_Object StepperMotor42_InitializeGroup(TIM_Object timer) {
    StepperMotor42Group_Object newStepperMotor42GroupObject = (StepperMotor42Group_Object)MeM_Request(sizeof(struct StepperMotor42Group_typeDef));
    TIM_InitializeObject(timer, 1000, StepperMotor42_StepHandler, (GenericObject)newStepperMotor42GroupObject, 0, 0);
    newStepperMotor42GroupObject->object = EmptyObject;
}


void StepperMotor32_AddToGroup(StepperMotor42Group_Object group,StepperMotor42_Object motor){
    motor->next = EmptyObject;
    if(group->object==EmptyObject){
        group->object = motor;
        return;
    }
    StepperMotor42_Object obj = group->object;
    while(obj->next!=EmptyObject){
        obj = obj = obj->next;
    }
    obj->next = motor;
}


/**
 * @brief 步进电机旋转指定步数
 * 
 * @param motorObj 指定一个步进电机对象
 * @param direction 方向
 * @param step 步数
 */
void StepperMotor42_RotateStep(StepperMotor42_Object motorObj,RotateDirectionTypeDef direction,unsigned int step){
    motorObj->totalStep = step;
    motorObj->curStep = 0;
    motorObj->status = StepperMotor42_Status_Running;//开始运行

}

/**
 * @brief 步进电机旋转指定角度
 * 
 * @param motorObj 指定一个步进电机对象
 * @param direction 旋转方向
 * @param angle 角度
 */
void StepperMotor42_RotateAngle(StepperMotor42_Object motorObj,RotateDirectionTypeDef direction,float angle){
    uint32_t step = (uint32_t)(angle * 100) / motorObj->stepAngle;
    StepperMotor42_RotateStep(motorObj, direction, step);
}


/**
 * @brief 步进电机旋转指定圈数
 * 
 * @param motorObj 指定一个步进电机对象
 * @param direction 旋转方向
 * @param cycle 圈数
 */
void StepperMotor42_RotateCycle(StepperMotor42_Object motorObj,RotateDirectionTypeDef direction,float cycle){
    uint32_t angle;
    angle = (uint32_t)(cycle * 100) * 360;
    StepperMotor42_RotateStep(motorObj, direction, angle / motorObj->stepAngle);
}

/**
 * @brief 锁定一个步进电机
 *
 * @param motor 指定一个步进电机对象
 */
void StepperMotor42_Lock(StepperMotor42_Object MotorObject) {
    switch (MotorObject->step) {
        case 0: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 1);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
        } break;
        case 1: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 1);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 1);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
        } break;
        case 2: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 0);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 1);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
        } break;
        case 3: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 0);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 1);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 1);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
        } break;
        case 4: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 0);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 1);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
        } break;
        case 5: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 0);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 1);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 1);
        } break;
        case 6: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 0);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 1);
        } break;
        case 7: {
            GPIO_WritePin(MotorObject->GPIO_APositive, 1);
            GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
            GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
            GPIO_WritePin(MotorObject->GPIO_BNegative, 1);
        } break;
    }
    MotorObject->status = StepperMotor42_Status_Lock;
}

/**
 * @brief 停用一个步进电机（即断电）
 *
 * @param MotorObject 指定一个步进电机对象
 */
void StepperMotor42_Disable(StepperMotor42_Object MotorObject) {
    GPIO_WritePin(MotorObject->GPIO_APositive, 0);
    GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
    GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
    GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
    MotorObject->status = StepperMotor42_Status_Disable;
}

void StepperMotor42_StepHandler(GenericObject object) {
    StepperMotor42_Object MotorObject = ((StepperMotor42Group_Object)object)->object;
    while (MotorObject) {
        if (MotorObject->status == StepperMotor42_Status_Running) {
            switch (MotorObject->step) {
                case 0: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 1);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
                } break;
                case 1: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 1);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 1);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
                } break;
                case 2: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 1);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
                } break;
                case 3: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 1);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 1);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
                } break;
                case 4: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 1);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 0);
                } break;
                case 5: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 1);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 1);
                } break;
                case 6: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 1);
                } break;
                case 7: {
                    GPIO_WritePin(MotorObject->GPIO_APositive, 1);
                    GPIO_WritePin(MotorObject->GPIO_BPositive, 0);
                    GPIO_WritePin(MotorObject->GPIO_ANegative, 0);
                    GPIO_WritePin(MotorObject->GPIO_BNegative, 1);
                } break;
                default: {
                    MotorObject->step = 0;
                } break;
            }
            if (MotorObject->direction == RotateDirection_Clockwise) {
                MotorObject->step++;
            } else if (MotorObject->direction == RotateDirection_Counterclockwise) {
                MotorObject->step--;
            }
            MotorObject->curStep++;
            if (MotorObject->curStep == MotorObject->totalStep) { //转完了指定步数
                StepperMotor42_Lock(MotorObject);
                if(MotorObject->endCallback){
                    MotorObject->endCallback(MotorObject->endCallbackParam);
                }
            }
        }
        MotorObject = MotorObject->next;
    }
}