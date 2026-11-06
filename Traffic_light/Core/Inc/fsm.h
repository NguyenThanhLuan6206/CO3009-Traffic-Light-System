#ifndef FSM_H
#define FSM_H

#include <stdint.h>

// Các Mode của hệ thống
#define MODE_AUTOMATIC  1
#define MODE_TUNING_RED  2
#define MODE_TUNING_YELLOW 3
#define MODE_TUNING_GREEN  4

// Khởi tạo trạng thái FSM
void fsm_init(void);

// Hàm FSM chính được Scheduler gọi định kỳ (ví dụ: mỗi 10ms hoặc 50ms)
void fsm_run(void);

#endif
