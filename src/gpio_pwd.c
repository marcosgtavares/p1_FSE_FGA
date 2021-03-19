#include <stdio.h>
#include <softPwm.h>  
#include <wiringPi.h> 
#include "../inc/gpio_pwd.h"

int pin_res;
int pin_fan;

void init_pwd(int set_pin_res, int set_pin_fan){
    wiringPiSetup();
    pin_res=set_pin_res;
    pinMode(set_pin_res,OUTPUT);	
	softPwmCreate(set_pin_res,1,100);

    pin_fan=set_pin_fan;
    pinMode(set_pin_fan,OUTPUT);	
	softPwmCreate(set_pin_fan,1,100);
}

void control_temp(double pid_itst){
    if(pid_itst >= 0){
        softPwmWrite (pin_fan, 0);
        softPwmWrite (pin_res, pid_itst);
    }
    else{
        softPwmWrite (pin_res, 0);
        softPwmWrite (pin_fan, -pid_itst);
    }
}

