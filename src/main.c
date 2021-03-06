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

void time_sec(int signum){//Controlador de tempo
    t=1;
}

void end_exec(int signum){
    control_temp(0);//Desliga a ventuinha e a resistencia
    free(dev);//libera a memoria alocada para dev
    close_csv();//Fecha o arquivo csv
    fechaUART(uart0);//Fecha a uart caso tenha encerrado antes dessa parte no codigo
    endwin();//Fecha as janelas
    sleep(1);//Espera as ações aconterem
    exit(0);
}

int main(int argc, const char * argv[]) {
    signal(SIGALRM, time_sec);//Captura do sinal de tempo

    signal(SIGINT, end_exec);
    signal(SIGTSTP, end_exec);//Captura do sinal de termino

    float ti=0,tr=0,te=0;//Variaveis de temperatura

    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );//Captura de tempo para criação do nome do arquivo csv
    create_csv(asctime (timeinfo),0,0,0,0,0);//Criação do arquivo csv

    uart0 = abreUART();// Abre a uart para solicitar os dados de TE e TR

    wiringPiSetup(); // Necessario para o uso do barramento i2c

    struct bme280_data comp_data;
    dev = init_sensor();
    int rslt = stream_sensor_data_normal_mode(dev);//Inicialização e configuração inicial do sensor bme280
    dev->delay_us(100, dev->intf_ptr);
    int fd_bme280;

    int fd_lcd=lcd_init();//Inicialização inicial da tela lcd
    close(fd_lcd);
    char line1_string[10];
    char line2_string[20];// Strings para escria na tela lcd
    
    init_pwd(4,5);//Inicia a gpio e prepara o PWD nas portas correspondentes a 23 e 24 do gpio para o wiringpi
    double temp_intst;// Intensidade do PWD

    initscr();// Incia a janela do ncurses

    nodelay(stdscr, TRUE);// Para a captura das teclas dentro do loop
    
    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);
    
    WINDOW *interface = newwin(10,50,max_y/2-5,max_x/2-25);
    WINDOW *input = newwin(4,50,max_y/2+5,max_x/2-25);

    refresh();
    box(interface, 0, 0);
    box(input, 0, 0);
    mvwprintw(interface, 1, 13, "Temperatura Externa:");
    mvwprintw(interface, 2, 13, "Temperatura Interna:");
    mvwprintw(interface, 3, 11, "Temperatura Referencia:");
    mvwprintw(interface, 4, 14, "Intensidade PWD:");
    mvwprintw(interface, 5, 21, "Segundos:");
    mvwprintw(interface, 6, 24, "Kp:");
    mvwprintw(interface, 7, 24, "Ki:");
    mvwprintw(interface, 8, 24, "Kd:");
    mvwprintw(input, 1, 12, "Input Const/Referencia:");// Inicializa o texto inicial da janela
    wrefresh(interface);
    wrefresh(input);
    char f_char[10];// Para a captura de characteres usando getch()
    int j=0,i=0;// j é o contador de characteres em f_char e i é o contador de tempo
    int use_ptc=1;// Usar o potenciometro ou não
    int type_param=0;// String f_char pronta ou não
    double curr_kp=5,curr_ki=1,curr_kd=5;
    while(1){
        if(t==1){//t é igual a um de começo e é igual a um a cada segundo
            move(max_y/2+6,max_x/2+10+j);
            t=0;
            alarm(1);//Primeira chamada da função alarm

            if(two_sec==2){//A cada 2 segundos captura o tempo
                time ( &rawtime );
                timeinfo = localtime ( &rawtime );
            }
            
            fd_bme280=set_i2c_addr_sensor();//Abre a porta do sensor bme280
		    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
            te = comp_data.temperature;
            close(fd_bme280);

            ti = solrecData(uart0, 0xC1);//Solicita TE
            if(use_ptc==1){
                tr = solrecData(uart0, 0xC2);//Solicita TR
            }
            if(type_param==1){//String f_cha pronta ou não
                type_param=0;
                
                if(f_char[0]=='k'){//Caso seja k o proximo caractere é checado para detectar a constante a ser alterada
                    if(strstr(f_char+2, "d")!=NULL || strstr(f_char+2, "i")!=NULL || strstr(f_char+2, "k")!=NULL || strstr(f_char+2, "p")!=NULL ){
                        mvwprintw(input, 1, 12, "Input Const/Referencia:N       ");
                        wrefresh(input);
                    }
                    else if(f_char[1]=='p'){
                        curr_kp=atof(f_char+2);  
                    }
                    else if(f_char[1]=='i'){
                        curr_ki=atof(f_char+2);
                    }
                    else if(f_char[1]=='d'){
                        curr_kd=atof(f_char+2);
                    }
                    else{
                        mvwprintw(input, 1, 12, "Input Const/Referencia:N       ");
                        wrefresh(input);
                    }
                    pid_configura_constantes(curr_kp,curr_ki,curr_kd);
                    
                }
                else if(f_char[0]=='p'){//Caso seja p a TR vira do potenciometro
                    tr = solrecData(uart0, 0xC2);
                    use_ptc=1;
                }
                else if(strstr(f_char, "d")==NULL && strstr(f_char, "i")==NULL && strstr(f_char, "k")==NULL && strstr(f_char, "p")==NULL ){//Nao possui de 'i','d','k' e 'p'
                    use_ptc=0;
                    tr=(float)atof(f_char);
                }
                else{
                    mvwprintw(input, 1, 12, "Input Const/Referencia:N       ");
                    wrefresh(input);
                }
                
            }
            if(tr<te){
                tr=te;
            }
            if(tr>100){
                tr=100;
            }

            snprintf(line1_string, 9, "TR:%0.1lf", tr);//Prepara as strings para escrever na tela lcd
            snprintf(line2_string, 17, "TI:%0.1lf TE:%0.1lf", ti, te);

            fd_lcd=set_i2c_addr_lcd();//Abre a porta da tela lcd
            lcdLoc(0x80);
            typeln(line1_string);
            lcdLoc(0xC0); 
            typeln(line2_string);
            close(fd_lcd);

            pid_atualiza_referencia(tr);
            temp_intst = pid_controle((double)ti);
            //printf("|%lf|\n",temp_intst);
            control_temp(temp_intst);//Controla o pwd com base na intensidade do PWD

            if(two_sec==2){//Adiciona ao csv à cada 2 segundos
                two_sec=0;
                if(temp_intst>=0){
                    create_csv(asctime (timeinfo),ti,te,tr,temp_intst,0);
                }
                else{
                    create_csv(asctime (timeinfo),ti,te,tr,0,(-1)*(temp_intst));
                }
            }

            mvwprintw(interface, 1, 13, "Temperatura Interna: %.2f",ti);//Atualiza os valores na janela
            mvwprintw(interface, 2, 13, "Temperatura Externa: %.2f",te);
            mvwprintw(interface, 3, 11, "Temperatura Referencia: %.2f",tr);
            mvwprintw(interface, 4, 14, "Intensidade PWD:        ");
            mvwprintw(interface, 4, 14, "Intensidade PWD: %.2f",temp_intst);
            mvwprintw(interface, 5, 21, "Segundos:%d",i);
            mvwprintw(interface, 6, 24, "Kp:%.2f",curr_kp);
            mvwprintw(interface, 7, 24, "Ki:%.2f",curr_ki);
            mvwprintw(interface, 8, 24, "Kd:%.2f",curr_kd);
            wrefresh(interface);
            wrefresh(input);
            i++;
            two_sec++;
            
        }
        if( type_param==0){//Pronto ou não para digitar mais
            if((f_char[j]=getch())=='\n'){// Finaliza a string caso receba \n
                f_char[j]='\0';
                wrefresh(input);
                mvwprintw(input, 2, 12, "Quando VAR  mudou(s):%d",i);
                j=0; 
                type_param=1;//String pronta
            }
            /*Estava recebendo algum tipo de input da raspberry então tive que limitar os caracteres aceitos*/
            else if((f_char[j]>=48 && f_char[j]<=57) || f_char[j]==112 || f_char[j]==46 || f_char[j]==100 || f_char[j]==105 || f_char[j]==107){
                if(j==0 ){// Necessario para controlar o cursor
                    mvwprintw(input, 1, 12, "Input Const/Referencia:%c            ",f_char[0]);
                    wrefresh(input);
                }
                j++;//Incrementa a posição na string f_char
            }
        }
        wrefresh(input);

    }

    return 0;
}
