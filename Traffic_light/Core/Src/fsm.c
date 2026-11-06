#include "fsm.h"
#include "software_timer.h"
#include "button.h"
#include "traffic_light.h"
#include "lcd.h"
#include "dht20.h"
#include <stdio.h>

// Biến quản lý Chế độ (Mode) hiện tại
static uint8_t system_mode = MODE_AUTOMATIC;

// Thời gian mặc định cho các đèn (Đơn vị: Giây)
static uint8_t red_duration = 5;
static uint8_t yellow_duration = 2;
static uint8_t green_duration = 3;

// Giá trị tạm dùng khi Tinh chỉnh (Tuning)
static uint8_t temp_duration = 0;

// Trạng thái đèn cho 2 hướng trong Chế độ Tự động (MODE 1)
typedef enum {
    STATE_RED,
    STATE_YELLOW,
    STATE_GREEN
} TrafficState;

static TrafficState state_ns = STATE_RED;
static TrafficState state_we = STATE_GREEN;

// Biến đếm ngược hiển thị ra LCD (Đơn vị: Giây)
static int count_ns = 0;
static int count_we = 0;

/* --------------------------------------------------------------------------
 * HÀM KHỞI TẠO (fsm_init)
 * -------------------------------------------------------------------------- */
void fsm_init(void) {
    system_mode = MODE_AUTOMATIC;

    // Đơn vị giao thông Bắc-Nam khởi đầu bằng đèn Đỏ, Đông-Tây bằng đèn Xanh
    state_ns = STATE_RED;
    count_ns = red_duration;

    state_we = STATE_GREEN;
    count_we = green_duration;

    set_traffic_light_NS(RED_LIGHT);
    set_traffic_light_WE(GREEN_LIGHT);

    // Timer 0: Chu kỳ 1s đếm ngược thời gian đèn
    setTimer(0, 1000);

    // Timer 1: Chu kỳ 2s đọc cảm biến DHT20
    setTimer(1, 2000);

    lcd_init();
    lcd_clear();
}

/* --------------------------------------------------------------------------
 * XỬ LÝ MODE 1: TỰ ĐỘNG (MODE_AUTOMATIC)
 * -------------------------------------------------------------------------- */
static void fsm_automatic_run(void) {
    // 1. Cập nhật đếm ngược mỗi 1 Giây (dựa vào Timer 0)
    if (timer_flag[0] == 1) {
        setTimer(0, 1000); // Reset timer 1s

        count_ns--;
        count_we--;

        // --- Xử lý chuyển trạng thái hướng Bắc - Nam ---
        if (count_ns <= 0) {
            switch (state_ns) {
                case STATE_RED:
                    state_ns = STATE_GREEN;
                    count_ns = green_duration;
                    set_traffic_light_NS(GREEN_LIGHT);
                    break;
                case STATE_GREEN:
                    state_ns = STATE_YELLOW;
                    count_ns = yellow_duration;
                    set_traffic_light_NS(YELLOW_LIGHT);
                    break;
                case STATE_YELLOW:
                    state_ns = STATE_RED;
                    count_ns = red_duration;
                    set_traffic_light_NS(RED_LIGHT);
                    break;
            }
        }

        // --- Xử lý chuyển trạng thái hướng Đông - Tây ---
        if (count_we <= 0) {
            switch (state_we) {
                case STATE_RED:
                    state_we = STATE_GREEN;
                    count_we = green_duration;
                    set_traffic_light_WE(GREEN_LIGHT);
                    break;
                case STATE_GREEN:
                    state_we = STATE_YELLOW;
                    count_we = yellow_duration;
                    set_traffic_light_WE(YELLOW_LIGHT);
                    break;
                case STATE_YELLOW:
                    state_we = STATE_RED;
                    count_we = red_duration;
                    set_traffic_light_WE(RED_LIGHT);
                    break;
            }
        }
    }

    // 2. Đọc cảm biến DHT20 mỗi 2 Giây (dựa vào Timer 1)
    if (timer_flag[1] == 1) {
        setTimer(1, 2000);
        dht20_read();
    }

    // 3. Hiển thị lên màn hình LCD
    char str_line1[17];
    char str_line2[17];

    // Dòng 1: Đếm ngược tín hiệu đèn 2 hướng
    sprintf(str_line1, "NS:%02d s | WE:%02d s", count_ns, count_we);
    // Dòng 2: Nhiệt độ và Độ ẩm từ DHT20
    sprintf(str_line2, "T:%.1fC | H:%.1f%%", dht20_get_temperature(), dht20_get_humidity());

    lcd_set_cursor(0, 0);
    lcd_print_string(str_line1);
    lcd_set_cursor(1, 0);
    lcd_print_string(str_line2);

    // 4. Chuyển Mode khi nhấn Button 1 (Chuyển sang Mode Tinh chỉnh Đèn Đỏ)
    if (isButtonPressed(0)) {
        system_mode = MODE_TUNING_RED;
        temp_duration = red_duration;
        lcd_clear();
        setTimer(2, 250); // Timer 2 dùng để nhấp nháy LED trong Tuning mode
    }
}

/* --------------------------------------------------------------------------
 * XỬ LÝ MODE 2, 3, 4: TINH CHỈNH THỜI GIAN (MODE_TUNING)
 * -------------------------------------------------------------------------- */
static void fsm_tuning_run(void) {
    // Timer nhấp nháy LED hiển thị trạng thái đang tinh chỉnh (250ms)
    if (timer_flag[2] == 1) {
        setTimer(2, 250);

        // Đảo trạng thái hiển thị đèn theo Mode đang chọn
        static uint8_t toggle = 0;
        toggle = !toggle;

        if (system_mode == MODE_TUNING_RED) {
            set_traffic_light_NS(toggle ? RED_LIGHT : OFF_LIGHT);
            set_traffic_light_WE(toggle ? RED_LIGHT : OFF_LIGHT);
        } else if (system_mode == MODE_TUNING_YELLOW) {
            set_traffic_light_NS(toggle ? YELLOW_LIGHT : OFF_LIGHT);
            set_traffic_light_WE(toggle ? YELLOW_LIGHT : OFF_LIGHT);
        } else if (system_mode == MODE_TUNING_GREEN) {
            set_traffic_light_NS(toggle ? GREEN_LIGHT : OFF_LIGHT);
            set_traffic_light_WE(toggle ? GREEN_LIGHT : OFF_LIGHT);
        }
    }

    // --- Xử lý Nút 2: Tăng thời gian (Hỗ trợ cả Nhấn đơn & Nhấn giữ) ---
    if (isButtonPressed(1) || isButtonLongPressed(1)) {
        temp_duration++;
        if (temp_duration > 99) temp_duration = 1; // Giới hạn từ 1 đến 99 giây
    }

    // --- Xử lý Nút 3: Lưu giá trị (SET / CONFIRM) ---
    if (isButtonPressed(2)) {
        if (system_mode == MODE_TUNING_RED) {
            red_duration = temp_duration;
        } else if (system_mode == MODE_TUNING_YELLOW) {
            yellow_duration = temp_duration;
        } else if (system_mode == MODE_TUNING_GREEN) {
            green_duration = temp_duration;
        }
    }

    // --- Xử lý Màn hình LCD hiển thị chế độ Tuning ---
    char str_line1[17];
    char str_line2[17];

    if (system_mode == MODE_TUNING_RED) {
        sprintf(str_line1, "MODE: TUNING RED ");
    } else if (system_mode == MODE_TUNING_YELLOW) {
        sprintf(str_line1, "MODE: TUNING YEL ");
    } else if (system_mode == MODE_TUNING_GREEN) {
        sprintf(str_line1, "MODE: TUNING GRE ");
    }

    sprintf(str_line2, "VAL: %02d sec       ", temp_duration);

    lcd_set_cursor(0, 0);
    lcd_print_string(str_line1);
    lcd_set_cursor(1, 0);
    lcd_print_string(str_line2);

    // --- Xử lý Nút 1: Chuyển Mode tiếp theo ---
    if (isButtonPressed(0)) {
        lcd_clear();
        if (system_mode == MODE_TUNING_RED) {
            system_mode = MODE_TUNING_YELLOW;
            temp_duration = yellow_duration;
        } else if (system_mode == MODE_TUNING_YELLOW) {
            system_mode = MODE_TUNING_GREEN;
            temp_duration = green_duration;
        } else if (system_mode == MODE_TUNING_GREEN) {
            // Quay về Mode 1 Tự động
            fsm_init();
        }
    }
}

/* --------------------------------------------------------------------------
 * HÀM CHẠY FSM TỔNG THỂ (Được Scheduler Dispatch)
 * -------------------------------------------------------------------------- */
void fsm_run(void) {
    switch (system_mode) {
        case MODE_AUTOMATIC:
            fsm_automatic_run();
            break;

        case MODE_TUNING_RED:
        case MODE_TUNING_YELLOW:
        case MODE_TUNING_GREEN:
            fsm_tuning_run();
            break;

        default:
            fsm_init();
            break;
    }
}
