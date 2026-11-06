#include "dht20.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;
#define DHT20_ADDR (0x38 << 1) // Địa chỉ I2C của DHT20

static float temperature = 0.0f;
static float humidity = 0.0f;

void dht20_init(void) {
    uint8_t read_buf[1];
    HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, read_buf, 1, 100);
    // Kiểm tra trạng thái khởi tạo nếu cần (Bit 3 phải bằng 1)
}

void dht20_read(void) {
    uint8_t send_buf[3] = {0xAC, 0x33, 0x00};
    uint8_t read_buf[7];

    // Gửi lệnh đo
    HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, send_buf, 3, 100);

    // Đợi 80ms theo datasheet (trong Scheduler sẽ gọi hàm này theo chu kỳ 2s)
    HAL_Delay(80);

    // Đọc 7 byte dữ liệu trả về
    if (HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, read_buf, 7, 100) == HAL_OK) {
        if ((read_buf[0] & 0x80) == 0) { // Kiểm tra bit bận (busy bit)
            uint32_t raw_humidity = ((uint32_t)read_buf[1] << 12) | ((uint32_t)read_buf[2] << 4) | ((uint32_t)read_buf[3] >> 4);
            uint32_t raw_temperature = (((uint32_t)read_buf[3] & 0x0F) << 16) | ((uint32_t)read_buf[4] << 8) | (uint32_t)read_buf[5];

            humidity = ((float)raw_humidity / 1048576.0f) * 100.0f;
            temperature = ((float)raw_temperature / 1048576.0f) * 200.0f - 50.0f;
        }
    }
}

float dht20_get_temperature(void) {
    return temperature;
}

float dht20_get_humidity(void) {
    return humidity;
}
