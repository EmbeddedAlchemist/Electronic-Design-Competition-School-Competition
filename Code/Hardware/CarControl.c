#include "CarControl.h"
#include "trigonometricFunctionTable.h"

static const float Pi = 3.14159265;


/**
 * @brief 生成一个小车对象
 * 
 * @param leftWheel 小车的左轮，初始化后的步进电机对象
 * @param rightWheel 小车的右轮，初始化后的步进电机对象
 * @param wheelRadius 轮半径，单位mm
 * @param halfWheelDistance 两轮之间一半距离，单位mm
 * @return Car_Object 
 */
Car_Object Car_Initialize(StepperMotor42_Object leftWheel, StepperMotor42_Object rightWheel,uint32_t wheelRadius, uint32_t halfWheelDistance) {
    Car_Object newCarObj = (Car_Object)MeM_Request(sizeof(struct Car_TypeDef));
    newCarObj->leftWheel = leftWheel;
    newCarObj->rightWheel = rightWheel;
    newCarObj->wheelRadius = wheelRadius;
    newCarObj->lastTaskIndex = 0;
    newCarObj->locateAngel = 0;
    newCarObj->locateX = 0;
    newCarObj->locateY = 0;
    newCarObj->taskSequence = EmptyObject;
    newCarObj->wheelHalfDistance = halfWheelDistance;
    return newCarObj;
}

/**
 * @brief 根据位置更新车车位置，在每次任务执行都应通过这个函数更新位置
 *
 * @param car
 * @param task
 */
void Car_UpdateLocation(Car_Object car, Car_TaskObject task) {
    if (task->type == Car_TaskType_Move) {
        //前进后退只影响XY坐标
        car->locateX += task->param * cosTable(car->locateAngel);
        car->locateY += task->param * sinTable(car->locateAngel);
    }
    if (task->type == Car_TaskType_Rotate) {
        //转向只影响角度
        car->locateAngel += task->param;
        //归置到0至359度的范围
        while (car->locateAngel >= 360)
            car->locateAngel -= 360;
        while (car->locateAngel < 0)
            car->locateAngel += 360;
    }
}

/**
 * @brief 任务执行
 *
 * @param task
 */
void Car_TaskHandler(Car_Object car, Car_TaskObject task) {
    Car_UpdateLocation(car, task);
    //轮子旋转的圈数
    float cycle;
    if (task->type == Car_TaskType_Move) {
        cycle = task->param / (2 * Pi * car->wheelRadius);
        StepperMotor42_RotateCycle(car->leftWheel, RotateDirection_Counterclockwise, cycle);
        StepperMotor42_RotateCycle(car->rightWheel, RotateDirection_Clockwise, cycle);
    } else if (task->type = Car_TaskType_Rotate) {
        float distance = 2 * Pi * car->wheelHalfDistance * task->param / 360;
        cycle = distance / (2 * Pi * car->wheelRadius);
        StepperMotor42_RotateCycle(car->leftWheel, RotateDirection_Clockwise, cycle);
        StepperMotor42_RotateCycle(car->rightWheel, RotateDirection_Clockwise, cycle);
    }
}

/**
 * @brief 轮子任务结束回调函数
 *
 * @param car
 * @param pos
 */

void Car_WheelTaskEndCallback(Car_Object car, uint32_t pos) {
    if (pos == 0) // zuol
        car->taskSequence->leftWheelTaskEnd = true;
    else if (pos == 1) //右轮
        car->taskSequence->rightWheelTaskEnd = true;
    if (car->taskSequence->leftWheelTaskEnd && car->taskSequence->rightWheelTaskEnd) {
        //任务结束，准备下一任务
        Car_TaskObject nextTask = car->taskSequence->next;
        DistoryObject(car->taskSequence);
        car->taskSequence = nextTask;
        Car_TaskHandler(car, nextTask);
    }
}
/**
 * @brief 左轮转动结束回调函数
 *
 * @param obj
 */
void Car_LeftWheelEndCallback(GenericObject obj) {
    Car_WheelTaskEndCallback((Car_Object)obj, 0);
}

/**
 * @brief 右轮转动结束回调函数
 *
 * @param obj
 */
void Car_RigntWheelEndCallback(GenericObject obj) {
    Car_WheelTaskEndCallback((Car_Object)obj, 1);
}

/**
 * @brief 设置车车在场地的位置
 *
 * @param car 指定一个车车
 * @param x
 * @param y
 * @param angel
 */
void Car_SetLocation(Car_Object car, int32_t x, int32_t y, int32_t angel) {
    car->locateX = x;
    car->locateY = y;
    car->locateAngel = angel;
    //归置到0至359度的范围
    while (car->locateAngel >= 360)
        car->locateAngel -= 360;
    while (car->locateAngel < 0)
        car->locateAngel += 360;
}

/**
 * @brief
 *
 * @param car
 * @param type
 * @param param
 * @return uint32_t 任务序号
 */

uint32_t Car_AddTask(Car_Object car, Car_TaskTypeTypeDef type, int32_t param) {
    Car_TaskObject newTaskObj = (Car_TaskObject)MeM_Request(sizeof(struct Car_TaskTypeDef));
    newTaskObj->index = car->lastTaskIndex + 1;
    newTaskObj->type = type;
    newTaskObj->param = param;
    newTaskObj->next = EmptyObject;
    newTaskObj->leftWheelTaskEnd = false;
    newTaskObj->rightWheelTaskEnd = false;
    if (car->taskSequence == EmptyObject) { //没有任务
        car->taskSequence = newTaskObj;
        Car_TaskHandler(car, newTaskObj);
    } else {
        Car_TaskObject tempObj = car->taskSequence;
        while (tempObj->next != EmptyObject) {
            tempObj = tempObj->next;
        }
        tempObj->next = newTaskObj;
    }
    return newTaskObj->index;
}

/**
 * @brief
 *
 * @param car 指定一个车车对象
 * @param distance 距离，正为前进，负为倒退
 */
uint32_t Car_MoveToware(Car_Object car, int32_t distance) {
    return Car_AddTask(car, Car_TaskType_Move, distance);
}

/**
 * @brief 车车旋转某个角度
 *
 * @param car
 * @param angel 正为逆时针，负为顺时针
 */
uint32_t Car_TurnAngel(Car_Object car, int32_t angel) {
    return Car_AddTask(car, Car_TaskType_Rotate, angel);
}

/**
 * @brief 车车转至某个角度
 *
 * @param car
 * @param angel 单位°，以x轴方向为0度
 */
uint32_t Car_TurnToAngel(Car_Object car, int32_t angel) {
    //归置到0至359度
    while (angel >= 360)
        angel -= 360;
    while (angel < 0)
        angel += 360;
    return Car_TurnAngel(car, angel - car->locateAngel);
}

/**
 * @brief 车车移动到指定位置
 *
 * @param car 指定一个车车对象
 * @param x X坐标
 * @param y Y坐标
 * @param angel 朝向角
 */
uint32_t Car_MoveToPosition(Car_Object car, int32_t x, int32_t y, int32_t angel) {
    Car_TurnToAngel(car, 0);               //先转至0度
    Car_MoveToware(car, x - car->locateX); // x轴
    Car_TurnToAngel(car, 90);
    Car_MoveToware(car, y - car->locateY); // Y轴
    return Car_TurnToAngel(car, angel);
}

Car_TaskStatusTypeDef Car_GetTaskStatus(Car_Object car, uint32_t index) {
    if (car->lastTaskIndex < index) { //任务不存在
        return Car_TaskStatus_Error;
    } else {
        if (car->taskSequence == EmptyObject)
            return Car_TaskStatus_Done;
        else if (car->taskSequence->index > index)
            return Car_TaskStatus_Done;
        else if (car->taskSequence->index == index)
            return Car_TaskStatus_Running;
        else
            return Car_TaskStatus_Waiting;
    }
}
