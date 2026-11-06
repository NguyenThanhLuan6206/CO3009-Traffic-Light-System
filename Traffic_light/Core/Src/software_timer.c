#include "software_timer.h"

// Khởi tạo mảng cờ báo (flag) và mảng biến đếm (counter) cho các timer
uint8_t timer_flag[NUM_TIMERS] = {0};
uint32_t timer_counter[NUM_TIMERS] = {0};

// Chu kỳ ngắt Timer của hệ thống là 10ms
#define TICK 10

/* --------------------------------------------------------
 * 1. Hàm cài đặt thời gian đếm ngược
 * -------------------------------------------------------- */
void setTimer(uint8_t index, uint32_t duration) {
    if (index >= NUM_TIMERS) return; // Bảo vệ tránh truy cập ngoài mảng

    // Chia cho chu kỳ TICK (10ms) để quy đổi từ mili-giây (ms) sang số lần đếm
    timer_counter[index] = duration / TICK;

    // Đặt cờ về 0 để báo hiệu là timer đang đếm, chưa hoàn thành
    timer_flag[index] = 0;
}

/* --------------------------------------------------------
 * 2. Hàm thực thi timer (Được Scheduler gọi mỗi 10ms)
 * -------------------------------------------------------- */
void timer_run(void) {
    for (int i = 0; i < NUM_TIMERS; i++) {
        // Chỉ xử lý những timer nào đang có giá trị đếm lớn hơn 0
        if (timer_counter[i] > 0) {
            timer_counter[i]--; // Giảm biến đếm đi 1 đơn vị

            // Nếu biến đếm về 0, bật cờ báo hiệu thời gian đã hết
            if (timer_counter[i] == 0) {
                timer_flag[i] = 1;
            }
        }
    }
}
