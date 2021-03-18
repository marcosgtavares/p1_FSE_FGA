#include "bme280.h"
#include "bme280_i2c.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
//#include <linux/i2c-dev.h>
//#include <sys/ioctl.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>


//#define IIC_Dev  "/dev/i2c-1"

int fd;

void user_delay_ms(uint32_t period, void *intf_ptr){
  usleep(period*1000);
}

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr){
  write(fd, &reg_addr,1);
  read(fd, reg_data, len);
  return 0;
}

int8_t user_i2c_write(uint8_t reg_addr, const unsigned char *reg_data, uint32_t len, void *intf_ptr){
  uint8_t *buf;
  buf = malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, reg_data, len);
  write(fd, buf, len +1);
  free(buf);
  return 0;
}
//void print_sensor_data(struct bme280_data *comp_data){
//	printf("temperature:%0.2f*C   pressure:%0.2fhPa   humidity:%0.2f%%\r\n",comp_data->temperature, comp_data->pressure/100, comp_data->humidity);
//}
int8_t stream_sensor_data_normal_mode(struct bme280_dev *dev){
	int8_t rslt;
	uint8_t settings_sel;
	//struct bme280_data comp_data;

	/* Recommended mode of operation: Indoor navigation */
	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev->settings.filter = BME280_FILTER_COEFF_16;
	dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);

	//printf("Temperature, Pressure, Humidity\r\n");
	//while (1) {
		/* Delay while the sensor completes a measurement */
		//dev->delay_us(1000, dev->intf_ptr);
		//rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
		//print_sensor_data(&comp_data);
	//}

	return rslt;
}

struct bme280_dev* init_sensor(){
    int8_t rslt = BME280_OK;
    uint8_t dev_addr = BME280_I2C_ADDR_PRIM;
    struct bme280_dev *dev=(struct bme280_dev *)malloc(sizeof(struct bme280_dev));

    if (wiringPiSetup () == -1) exit (1); 
    fd = wiringPiI2CSetup(0x76);

    /*if ((fd = open(IIC_Dev, O_RDWR)) < 0) {
        printf("Failed to open the i2c bus");
        exit(1);
    }
    if (ioctl(fd, I2C_SLAVE, 0x76) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        exit(1);
    }*/
    dev->intf_ptr = &dev_addr;
    dev->intf = BME280_I2C_INTF;
    dev->read = &user_i2c_read;
    dev->write = &user_i2c_write;
    dev->delay_us = &user_delay_ms;
    
    rslt = bme280_init(dev);
    return dev;
}

void set_i2c_addr_sensor(struct bme280_dev *dev){
  if (wiringPiSetup () == -1) exit (1); 
  fd = wiringPiI2CSetup(0x76);
  int rslt;
  rslt = bme280_init(dev);
}