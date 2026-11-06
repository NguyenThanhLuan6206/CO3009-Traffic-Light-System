#include "button.h"
#include "main.h" // Để lấy HAL và định nghĩa chân

// Định nghĩa trạng thái nút nhấn (giả sử nút nhấn nối xuống GND, nhấn là mức 0)
#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

// Cấu hình Port và Pin cho 4 nút nhấn (Bạn cần thay đổi lại cho khớp với .ioc sau)
static GPIO_TypeDef* BUTTON_PORT[NUM_BUTTONS] = {BTN_1_GPIO_Port, BTN_2_GPIO_Port, BTN_3_GPIO_Port, BTN_4_GPIO_Port};
static uint16_t BUTTON_PIN[NUM_BUTTONS]       = {BTN_1_Pin, BTN_2_Pin, BTN_3_Pin, BTN_4_Pin};

// Các buffer dùng cho việc chống dội (lấy từ logic của bạn)
static GPIO_PinState debounceBuffer1[NUM_BUTTONS];
static GPIO_PinState debounceBuffer2[NUM_BUTTONS];
static GPIO_PinState debounceBuffer3[NUM_BUTTONS];

// Trạng thái hiện tại của nút nhấn sau khi đã chống dội
static GPIO_PinState buttonState[NUM_BUTTONS];

// Các cờ báo hiệu
static uint8_t flagForButtonPressed[NUM_BUTTONS] = {0};
static uint8_t flagForButtonLongPressed[NUM_BUTTONS] = {0};

// Biến đếm thời gian dùng cho tính năng nhấn giữ (Long press)
// Nếu Timer ngắt mỗi 10ms, đếm đến 200 tức là đã giữ nút được 2 giây
static uint16_t counterForButtonPress3s[NUM_BUTTONS] = {0};

/* --------------------------------------------------------
 * 1. Khởi tạo trạng thái ban đầu
 * -------------------------------------------------------- */
void button_init(void) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        debounceBuffer1[i] = BUTTON_IS_RELEASED;
        debounceBuffer2[i] = BUTTON_IS_RELEASED;
        debounceBuffer3[i] = BUTTON_IS_RELEASED;
        buttonState[i]     = BUTTON_IS_RELEASED;
        flagForButtonPressed[i] = 0;
        flagForButtonLongPressed[i] = 0;
        counterForButtonPress3s[i] = 0;
    }
}

/* --------------------------------------------------------
 * 2. Hàm đọc và xử lý chống dội (Được gọi mỗi 10ms từ Scheduler)
 * -------------------------------------------------------- */
void button_read(void) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        // Dịch chuyển buffer (Logic chống dội từ input_reading.c)
        debounceBuffer1[i] = debounceBuffer2[i];
        debounceBuffer2[i] = debounceBuffer3[i];
        debounceBuffer3[i] = HAL_GPIO_ReadPin(BUTTON_PORT[i], BUTTON_PIN[i]);

        // Nếu 3 lần đọc liên tiếp có cùng một giá trị, tín hiệu đã ổn định
        if ((debounceBuffer1[i] == debounceBuffer2[i]) &&
            (debounceBuffer2[i] == debounceBuffer3[i])) {

            // Nếu trạng thái mới khác với trạng thái trước đó
            if (buttonState[i] != debounceBuffer3[i]) {
                buttonState[i] = debounceBuffer3[i];

                // Xử lý sự kiện "Vừa mới nhấn xuống"
                if (buttonState[i] == BUTTON_IS_PRESSED) {
                    flagForButtonPressed[i] = 1;
                    // Reset biến đếm nhấn giữ khi vừa nhấn xuống
                    counterForButtonPress3s[i] = 200; // Đếm ngược 2s (200 * 10ms)
                }
                else {
                    // Xử lý sự kiện nhả nút (nếu cần)
                    counterForButtonPress3s[i] = 0;
                    flagForButtonLongPressed[i] = 0;
                }
            }
            else { // Nút vẫn đang giữ trạng thái cũ
                // Nếu nút đang bị giữ đè
                if (buttonState[i] == BUTTON_IS_PRESSED) {
                    counterForButtonPress3s[i]--;

                    // Nếu thời gian đếm ngược về 0 (đã giữ đủ lâu)
                    if (counterForButtonPress3s[i] == 0) {
                        // Kích hoạt cờ nhấn giữ
                        flagForButtonLongPressed[i] = 1;

                        // Đặt lại thời gian đếm để kích hoạt liên tục
                        // (Ví dụ: tự động tăng/giảm số liên tục mỗi 0.5s)
                        counterForButtonPress3s[i] = 50;
                    }
                }
            }
        }
    }
}

/* --------------------------------------------------------
 * 3. Các hàm kiểm tra trạng thái cho FSM sử dụng
 * -------------------------------------------------------- */
uint8_t isButtonPressed(uint8_t index) {
    if (index >= NUM_BUTTONS) return 0;
    if (flagForButtonPressed[index] == 1) {
        flagForButtonPressed[index] = 0; // Xóa cờ sau khi đọc để không xử lý nhiều lần
        return 1;
    }
    return 0;
}

uint8_t isButtonLongPressed(uint8_t index) {
    if (index >= NUM_BUTTONS) return 0;
    if (flagForButtonLongPressed[index] == 1) {
        flagForButtonLongPressed[index] = 0;
        return 1;
    }
    return 0;
}
