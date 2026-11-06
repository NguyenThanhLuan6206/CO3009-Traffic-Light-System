#include "lcd.h"
#include "main.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
#define LCD_ADDR (0x27 << 1) // Địa chỉ I2C mặc định của module PCF8574 (tuỳ chọn 0x27 hoặc 0x3F)

static void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C;  // en=1, rs=0
    data_t[1] = data_u | 0x08;  // en=0, rs=0
    data_t[2] = data_l | 0x0C;  // en=1, rs=0
    data_t[3] = data_l | 0x08;  // en=0, rs=0
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, (uint8_t *) data_t, 4, 100);
}

static void lcd_send_data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D;  // en=1, rs=1
    data_t[1] = data_u | 0x09;  // en=0, rs=1
    data_t[2] = data_l | 0x0D;  // en=1, rs=1
    data_t[3] = data_l | 0x09;  // en=0, rs=1
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, (uint8_t *) data_t, 4, 100);
}

void lcd_init(void) {
    // Khởi tạo ở chế độ 4-bit
    HAL_Delay(50);
    lcd_send_cmd(0x30);
    HAL_Delay(5);
    lcd_send_cmd(0x30);
    HAL_Delay(1);
    lcd_send_cmd(0x32);
    HAL_Delay(10);

    lcd_send_cmd(0x28); // Chế độ 4-bit, 2 dòng, font 5x8
    lcd_send_cmd(0x08); // Tắt hiển thị
    lcd_send_cmd(0x01); // Xóa màn hình
    HAL_Delay(2);
    lcd_send_cmd(0x06); // Tự động tăng con trỏ
    lcd_send_cmd(0x0C); // Bật hiển thị, tắt con trỏ
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_send_cmd(addr);
}

void lcd_print_string(char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_print_number(int num) {
    char buffer[10];
    sprintf(buffer, "%d", num);
    lcd_print_string(buffer);
}
