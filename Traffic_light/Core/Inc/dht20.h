#ifndef DHT20_H
#define DHT20_H

#include <stdint.h>

// Hàm khởi tạo cảm biến DHT20
void dht20_init(void);

// Hàm giao tiếp I2C để đọc dữ liệu (Sẽ được gọi định kỳ bởi Scheduler hoặc Timer)
void dht20_read(void);

// Các hàm lấy giá trị nhiệt độ và độ ẩm đã được chuyển đổi để hiển thị
float dht20_get_temperature(void);
float dht20_get_humidity(void);

#endif
