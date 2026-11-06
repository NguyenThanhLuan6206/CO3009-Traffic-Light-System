#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

// Cấu trúc dữ liệu của một tác vụ (Task)
typedef struct {
    void (*pTask) (void); // pointer to the task function
    uint32_t Delay;       // remaining ticks (relative)
    uint32_t Period;      // period, 0 = one-shot task
    uint8_t RunMe;        // flag: task ready to run
    uint32_t TaskID;      // mã định danh của task
} sTask;

// Các hàm giao tiếp của bộ lập lịch
void SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunc) (void), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);

#endif /* SCHEDULER_H */
