#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

// Định nghĩa các trạng thái cho FSM Automatic
#define AUTO_INIT           0
#define AUTO_RED1_GREEN2    1
#define AUTO_RED1_AMBER2    2
#define AUTO_GREEN1_RED2    3
#define AUTO_AMBER1_RED2    4

// Định nghĩa trạng thái cho FSM Manual & Tuning (bạn có thể thêm sau)
#define MAN_INIT            10
#define TUNING_INIT         20

// Màu đèn giao thông
#define RED     0
#define GREEN   1
#define AMBER   2

// Các biến toàn cục (thời gian cấu hình cho mỗi đèn)
extern uint32_t time_red;
extern uint32_t time_green;
extern uint32_t time_amber;

// Biến lưu trạng thái hiện tại của hệ thống
extern uint8_t status;

#endif
