#ifndef BME280_I2C_H_
#define BME280_I2C_H_

#include "bme280.h"


void user_delay_ms(uint32_t period, void *intf_ptr);
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
int8_t user_i2c_write(uint8_t reg_addr, const unsigned char *reg_data, uint32_t len, void *intf_ptr);
void print_sensor_data(struct bme280_data *comp_data);
int8_t stream_sensor_data_normal_mode(struct bme280_dev *dev);
struct bme280_dev* init_sensor();
int set_i2c_addr_sensor();

#endif /* BME280_H_ */