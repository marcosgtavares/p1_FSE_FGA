#ifndef GPIO_PWD_H_
#define GPIO_PWD_H_

void init_pwd(int pin_res, int pin_fan);
void control_temp(double pid_itst);

#endif /* GPIO_PWD_H_ */