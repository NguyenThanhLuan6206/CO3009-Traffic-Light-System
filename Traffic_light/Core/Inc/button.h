#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define NUM_BUTTONS 4

// Hàm khởi tạo trạng thái ban đầu của nút nhấn
void button_init(void);

// Hàm đọc và xử lý chống dội (Sẽ được gọi như một Task trong Scheduler mỗi 10ms)
void button_read(void);

// Hàm kiểm tra xem nút có được nhấn hay không (trả về 1 nếu có)
uint8_t isButtonPressed(uint8_t index);

// Hàm kiểm tra xem nút có đang bị nhấn đè hay không
uint8_t isButtonLongPressed(uint8_t index);

#endif
