#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// Khởi tạo màn hình LCD
void lcd_init(void);

// Xóa toàn bộ nội dung trên màn hình
void lcd_clear(void);

// Đặt vị trí con trỏ (row: hàng, col: cột)
void lcd_set_cursor(uint8_t row, uint8_t col);

// In một chuỗi ký tự ra màn hình
void lcd_print_string(char *str);

// In một số nguyên ra màn hình (rất hữu ích khi hiển thị thời gian đếm ngược)
void lcd_print_number(int num);

#endif
