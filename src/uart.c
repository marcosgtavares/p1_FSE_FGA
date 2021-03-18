#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include <string.h>
#include "crc16.h"
#include "uart.h"


#define adrss 0x01
#define wriCod 0x23
#define solTI 0xC1
#define solTP 0xC2

int writeB(int uart0, unsigned char xA){
    unsigned char codigo_int[7]={adrss,wriCod,xA,0x01,0x00,0x04,0x02};//Preparando os primeiros 7 bytes
    unsigned char msg[9];
    printf("Buffers de memória criados!\n");
    short crc = calcula_CRC(codigo_int, 7);//Calculando o CRC com base nos primeiros 7 bytes
    printf("CRC calculado!\n");
    memcpy(msg, codigo_int, 7);//Preparando a mesagem ao colocar os primeiros 7 bytes
    printf("Mensagem criada P1\n");
    memcpy(&msg[7], &crc, 2);//Finalizando a mensagem ao colcoar os ultimos 2 bytes(CRC)
    printf("Mensagem criada P2\n");
    if (uart0 != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0, &msg[0], 9);
        if (count < 0)
        {
            printf("UART TX error\n");
            return -1;
        }
        else
        {
            printf("escrito.\n");
            return 1;
        }
    }
}
/*void writeBSend(int uart0, unsigned char xB, unsigned char *data){
    
    int dataSize = sizeof(data);//Tamanho em bytes dos dados a serem enviados
    unsigned char codePass[3]={adrss,reaCod,xB};//Preparando os primeiros 3 bytes da mensagem
    unsigned char *msg=(unsigned char *)malloc(dataSize+3);//Alocando a memoria para a mensagem sem o CRC
    printf("asdf");
    memcpy(msg, codePass, 3);//Adicionando os primeiros 3 bytes à mensagem
    memcpy(&msg[3], data, dataSize);//Adicionando os dados à mensagem
    printf("Buffers de memória criados!\n");
    short crc = calcula_CRC(msg, 3+dataSize);//Calculando o CRC
    printf("CRC calculado!\n");
    msg = realloc(msg, dataSize+5);//Realocando a memoria para a mensagem comportar o CRC
    memcpy(&msg[dataSize+3], &crc, 2);//Finalizando a mensagem ao adicionar os ultimos 2 bytes(CRC)
    printf("Mensagem criada\n");
    if (uart0 != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0, &msg[0], dataSize+5);
        if (count < 0)
        {
            writen=0;
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
            writen=1;
        }
    }

    sleep(1);
    free(msg);
}*/

float readB(int uart0){
    unsigned char rx_buffer[256];
    unsigned char *receiveC;
    float *dados;
    short receivedCrc;
    if (uart0 != -1)
    {
        // Read up to 255 characters from the port if they are there
        int rx_length = read(uart0, (void*)rx_buffer, 255);      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
            return -1;
        }
        else if (rx_length == 0)
        {   
            printf("Nenhum dado disponível.\n"); //No data waiting
            return -1;
        }
        else
        {
            //Bytes received
            receiveC = (unsigned char *)malloc((rx_length-2));//Alocando a memoria para os primeiros 3 bytes mais os dados
            memcpy(receiveC,rx_buffer,rx_length-2);
            short crc = calcula_CRC(receiveC, rx_length-2);//Calculando o CRC
            free(receiveC);
            memcpy(&receivedCrc,&rx_buffer[rx_length-2],2);//Recuperando o CRC do buffer
            if(receivedCrc==crc){//Comparando o CRC 
                dados = (float *)malloc(1*sizeof(float)); 
                printf("CRC valido.\n");
                memcpy(dados,&rx_buffer[3],rx_length-5);
                float val = *dados;
                free(dados);
                return val;
            }
            else{
                printf("CRC invalido.\n");
                return -1;
            }

        }
    }
}

int abreUART(){
    int uart0_filestream; 
    uart0_filestream =-1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);
    return uart0_filestream;

}
void fechaUART(int uart0){
    close(uart0);
}

float solrecData(int uart0, unsigned char xA){
    int i,j;
    j=0;
    do{
        i=writeB(uart0, xA);
        j++;
    }while ((i==-1) && j<10);
    j=0;
    float result;
    usleep(100000);
    do{
        result=readB(uart0);
        usleep(10000);
        j++;
    }while ((result==-1) && j<10);
    return result;
}
