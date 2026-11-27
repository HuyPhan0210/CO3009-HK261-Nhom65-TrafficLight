#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include "main.h"

#define DHT20_ADDR    (0x38 << 1)   // dia chi 7 bit 0x38, HAL can dich trai 1 bit
#define I2C_TIMEOUT   10  // ms, thoi gian cho toi da moi lenh I2C

void dht20_init(void); // goi 1 lan trong main(), truoc while(1)
void dht20_task(void); // task scheduler, goi moi 100 ms

float   dht20_get_temperature(void);   // do C
float   dht20_get_humidity(void); // %
uint8_t dht20_is_error(void); // 1 = cam bien loi / chua co so do

// Kiem tra loi bus I2C, dung chung cho DHT20 va LCD
void i2c_bus_check(HAL_StatusTypeDef st);
void i2c_bus_recover(void);

#endif /* INC_DHT20_H_ */
