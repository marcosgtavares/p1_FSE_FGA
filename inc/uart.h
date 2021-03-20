#ifndef UART_H_
#define UART_H_

int writeB(int uart0, unsigned char xA);
float readB(int uart0);
int abreUART();
void fechaUART(int uart0);
float solrecData(int uart0, unsigned char xA);


#endif /* UART_H_ */