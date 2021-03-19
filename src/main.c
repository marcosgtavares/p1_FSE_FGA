#include "../inc/uart.h"
#include "../inc/bme280_i2c.h"
#include "../inc/lcd_i2c.h"
#include "../inc/pid.h"
#include "../inc/gpio_pwd.h"
#include "../inc/csv_gen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <ncurses.h>

int t=1;
int two_sec=0;
int uart0;
struct bme280_dev *dev;

void time_sec(int signum){
    t=1;
}

void end_exec(int signum){
    control_temp(0);
    free(dev);
    close_csv();
    fechaUART(uart0);
    endwin();
    sleep(1);
    exit(0);
}

int main(int argc, const char * argv[]) {
    signal(SIGALRM, time_sec);

    signal(SIGINT, end_exec);
    signal(SIGTSTP, end_exec);

    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    create_csv(asctime (timeinfo),0,0,0,0,0);

    if (wiringPiSetup () == -1) exit (1); // Necessario para o uso do barramento i2c

    struct bme280_data comp_data;
    dev = init_sensor();
    int rslt = stream_sensor_data_normal_mode(dev);

    lcd_init();

    float ti,tr,te;
    ti=0;
    tr=0;
    te=0;

    char line1_string[10];
    char line2_string[20];
    
    double temp_intst;

    initscr();

    halfdelay(1);
    
    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);
    
    WINDOW *interface = newwin(10,50,max_y/2-5,max_x/2-25);
    WINDOW *input = newwin(3,50,max_y/2+5,max_x/2-25);

    refresh();
    box(interface, 0, 0);
    box(input, 0, 0);
    mvwprintw(interface, 1, 13, "Temperatura Externa:");
    mvwprintw(interface, 2, 13, "Temperatura Interna:");
    mvwprintw(interface, 3, 11, "Temperatura Referencia:");
    mvwprintw(interface, 4, 14, "Intensidade PWD:");
    mvwprintw(interface, 5, 20, "Segundos:");
    mvwprintw(input, 1, 12, "Temperatura Referencia:");
    wrefresh(interface);
    wrefresh(input);
    char f_char[10];
    int j=0;
    int i=0;
    int type_param=0;

    while(1){
        if(t==1){
            
            t=0;
            two_sec++;
            alarm(1);

            if(two_sec==2){
                time ( &rawtime );
                timeinfo = localtime ( &rawtime );
            }
            
            uart0 = abreUART();
            ti = solrecData(uart0, 0xC1);
            if(type_param==1){
                type_param=0;
                j=0;
                if(f_char[0]!='p'){
                    tr=(float)atof(f_char);
                    mvwprintw(input, 1, 12, "Temperatura Referencia:%.2f",tr);
                    wrefresh(input);
                    move(max_y/2+6,max_x/2+10);
                }
                else{
                    tr = solrecData(uart0, 0xC2);
                    mvwprintw(input, 1, 12, "Temperatura Referencia:%.2f",tr);
                    wrefresh(input);
                    move(max_y/2+6,max_x/2+10);
                }
                
            }
            if(tr==0){
                tr = solrecData(uart0, 0xC2);
            }
            fechaUART(uart0);
            
            set_i2c_addr_sensor(dev);
            int rslt = stream_sensor_data_normal_mode(dev);
            dev->delay_us(100, dev->intf_ptr);
		    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
            te = comp_data.temperature;

            snprintf(line1_string, 9, "TR:%0.1lf", tr);
            snprintf(line2_string, 17, "TI:%0.1lf TE:%0.1lf", ti, te);

            //printf("|%f|%f|%f|\n",tr,ti,te);

            set_i2c_addr_lcd();
            lcdLoc(0x80);
            typeln(line1_string);
            lcdLoc(0xC0); 
            typeln(line2_string);

            init_pwd(4,5);
            pid_atualiza_referencia(tr);
            temp_intst = pid_controle((double)ti);
            //printf("|%lf|\n",temp_intst);
            control_temp(temp_intst);

            if(two_sec==2){
                two_sec=0;
                if(temp_intst>=0){
                    create_csv(asctime (timeinfo),ti,te,tr,temp_intst,0);
                }
                else{
                    create_csv(asctime (timeinfo),ti,te,tr,0,(-1)*(temp_intst));
                }
            }

            mvwprintw(interface, 1, 13, "Temperatura Interna: %.2f",ti);
            mvwprintw(interface, 2, 13, "Temperatura Externa: %.2f",te);
            mvwprintw(interface, 3, 11, "Temperatura Referencia: %.2f",tr);
            mvwprintw(interface, 4, 14, "Intensidade PWD:        ");
            mvwprintw(interface, 4, 14, "Intensidade PWD: %.2f",temp_intst);
            mvwprintw(interface, 5, 18, "Segundos:%d",i);
            move(max_y/2+6,max_x/2+10+j);
            wrefresh(interface);
            i++;
            
        }
        if( type_param==0){
            if((f_char[j]=getch())=='\n'){
                f_char[j]='\0';
                move(max_y/2+6,max_x/2+10+j);
                wrefresh(input);
                j=0; 
                type_param=1;
            }
            else if((f_char[j]!=-1) && ((f_char[j]>=48 && f_char[j]<=57) || (f_char[j]>=65 && f_char[j]<=90) || (f_char[j]>=97 && f_char[j]<=122) || f_char[j]==46)){
                if(j==0 ){
                    mvwprintw(input, 1, 12, "Input  T Referencia:%c            ",f_char[0]);
                    move(max_y/2+6,max_x/2+11);
                    wrefresh(input);
                }
                j++;
            }
        }
        

    }

    return 0;
}
