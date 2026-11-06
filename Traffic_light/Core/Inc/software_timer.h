#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>

// Khai báo số lượng timer tối đa cần dùng (ví dụ: 1 timer cho đèn, 1 cho LCD...)
#define NUM_TIMERS 4

extern uint8_t timer_flag[NUM_TIMERS];

// Hàm đặt thời gian cho timer (duration tính bằng ms hoặc tick)
void setTimer(uint8_t index, uint32_t duration);

// Hàm cập nhật timer (Sẽ được gọi như một Task trong Scheduler mỗi 10ms)
void timer_run(void);

#endif
