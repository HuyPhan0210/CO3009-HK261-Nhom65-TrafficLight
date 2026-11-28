#include "scheduler.h"
#include "main.h"

// Mảng luôn sắp xếp theo thời gian đến hạn.
// Delay = số tick tính từ task đứng trước.

static sTask SCH_tasks_G[SCH_MAX_TASKS];
unsigned char Error_code_G = 0;
static uint32_t next_task_id = 1;
static volatile uint32_t time_tick = 0;

void SCH_Init(void) {
    for (unsigned char i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = 0;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].TaskID = NO_TASK_ID;
    }
    Error_code_G = 0;
    next_task_id = 1;
    time_tick = 0;
}

// O(1): chỉ giảm task đầu tiên trong ngắt TIM2
void SCH_Update(void) {
    time_tick++;
    if (SCH_tasks_G[0].pTask != 0 && SCH_tasks_G[0].Delay > 0) {
        SCH_tasks_G[0].Delay--;
    }
}

// Chèn task vào đúng vị trí (delay và period tính theo tick)
static uint8_t SCH_Insert(void (*pFunction)(void), uint32_t delay_ticks, uint32_t period_ticks, uint32_t id) {
    unsigned char Index = 0;

    if (SCH_tasks_G[SCH_MAX_TASKS - 1].pTask != 0) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        return 0;
    }

    while (SCH_tasks_G[Index].pTask != 0 && delay_ticks >= SCH_tasks_G[Index].Delay) {
        delay_ticks -= SCH_tasks_G[Index].Delay;
        Index++;
    }

    for (unsigned char j = SCH_MAX_TASKS - 1; j > Index; j--) {
        SCH_tasks_G[j] = SCH_tasks_G[j - 1];
    }

    if (Index + 1 < SCH_MAX_TASKS && SCH_tasks_G[Index + 1].pTask != 0) {
        SCH_tasks_G[Index + 1].Delay -= delay_ticks;
    }

    SCH_tasks_G[Index].pTask = pFunction;
    SCH_tasks_G[Index].Delay = delay_ticks;
    SCH_tasks_G[Index].Period = period_ticks;
    SCH_tasks_G[Index].TaskID = id;
    return 1;
}

// Xóa task ở vị trí Index
static void SCH_Remove(unsigned char Index) {
    if (Index + 1 < SCH_MAX_TASKS && SCH_tasks_G[Index + 1].pTask != 0) {
        SCH_tasks_G[Index + 1].Delay += SCH_tasks_G[Index].Delay;
    }

    for (unsigned char j = Index; j < SCH_MAX_TASKS - 1; j++) {
        SCH_tasks_G[j] = SCH_tasks_G[j + 1];
    }

    SCH_tasks_G[SCH_MAX_TASKS - 1].pTask = 0;
    SCH_tasks_G[SCH_MAX_TASKS - 1].Delay = 0;
    SCH_tasks_G[SCH_MAX_TASKS - 1].Period = 0;
    SCH_tasks_G[SCH_MAX_TASKS - 1].TaskID = NO_TASK_ID;
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t delay_ms, uint32_t period_ms) {
    uint32_t delay_ticks = delay_ms / TICK;
    uint32_t period_ticks = period_ms / TICK;

    __disable_irq();
    uint32_t id = next_task_id;
    if (SCH_Insert(pFunction, delay_ticks, period_ticks, id) == 0) {
        __enable_irq();
        return NO_TASK_ID;
    }
    next_task_id++;
    if (next_task_id == NO_TASK_ID) next_task_id = 1;
    __enable_irq();

    return id;
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID == NO_TASK_ID) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        return 0;
    }

    __disable_irq();
    for (unsigned char Index = 0; Index < SCH_MAX_TASKS; Index++) {
        if (SCH_tasks_G[Index].pTask != 0 && SCH_tasks_G[Index].TaskID == taskID) {
            SCH_Remove(Index);
            __enable_irq();
            return 1;
        }
    }
    __enable_irq();

    Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
    return 0;
}

void SCH_Dispatch_Tasks(void) {
    while (1) {
        __disable_irq();
        if (SCH_tasks_G[0].pTask == 0 || SCH_tasks_G[0].Delay > 0) {
            __enable_irq();
            break;
        }

        sTask t = SCH_tasks_G[0];
        SCH_Remove(0);
        if (t.Period > 0) {
            SCH_Insert(t.pTask, t.Period, t.Period, t.TaskID);
        }
        __enable_irq();

        (*t.pTask)();
    }
}

uint32_t get_time(void) {
    return time_tick * TICK;
}
