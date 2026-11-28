#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_MAX_TASKS                 10
#define NO_TASK_ID                    0
#define TICK                          10  // Chu kỳ ngắt TIM2 = 10 ms

#define ERROR_SCH_TOO_MANY_TASKS      1
#define ERROR_SCH_CANNOT_DELETE_TASK  2

typedef struct {
    void (*pTask)(void);
    volatile uint32_t Delay;
    uint32_t Period;
    uint32_t TaskID;
} sTask;

extern unsigned char Error_code_G;

void SCH_Init(void);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t delay_ms, uint32_t period_ms);
uint8_t SCH_Delete_Task(uint32_t taskID);
uint32_t get_time(void);

#endif /* INC_SCHEDULER_H_ */
