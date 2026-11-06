#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdint.h>
#include "global.h" // Để lấy định nghĩa RED, GREEN, AMBER

// Hàm cài đặt trạng thái đèn cho một tuyến đường cụ thể
// Ví dụ: traffic_set(1, RED)
void traffic_set(uint8_t road, uint8_t color);

// Hàm tắt toàn bộ đèn (dùng khi khởi tạo hoặc chuyển mode)
void traffic_clear(uint8_t road);

#endif
