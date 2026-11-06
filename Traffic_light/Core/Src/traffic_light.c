#include "traffic_light.h"
#include "main.h"

/* --------------------------------------------------------
 * Cấu hình chân điều khiển từ sơ đồ Schematic:
 * Cụm 1 (Bắc - Nam): Chân D3, D4
 * Cụm 2 (Đông - Tây): Chân D5, D6
 * -------------------------------------------------------- */

void traffic_light_init(void) {
    // Đặt mặc định tắt hết đèn hoặc đưa về trạng thái Đỏ
    set_traffic_light_NS(RED_LIGHT);
    set_traffic_light_WE(RED_LIGHT);
}

// Bảng trạng thái tín hiệu cho cụm Bắc - Nam (D3, D4)
void set_traffic_light_NS(uint8_t state) {
    switch (state) {
        case RED_LIGHT: // Mã 01
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
            break;
        case YELLOW_LIGHT: // Mã 10
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
            break;
        case GREEN_LIGHT: // Mã 11
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
            break;
        case OFF_LIGHT: default: // Mã 00
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
            break;
    }
}

// Bảng trạng thái tín hiệu cho cụm Đông - Tây (D5, D6)
void set_traffic_light_WE(uint8_t state) {
    switch (state) {
        case RED_LIGHT: // Mã 01
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
            break;
        case YELLOW_LIGHT: // Mã 10
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
            break;
        case GREEN_LIGHT: // Mã 11
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
            break;
        case OFF_LIGHT: default: // Mã 00
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
            break;
    }
}
