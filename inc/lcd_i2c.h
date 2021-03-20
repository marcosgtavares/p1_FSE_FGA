
#ifndef LCD_I2C_H_
#define LCD_I2C_H_

int lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);
void typeInt(int i);
void typeFloat(float myFloat);
void lcdLoc(int line); //move cursor
void ClrLcd(void); // clr LCD return home
void typeln(const char *s);
void typeChar(char val);
int set_i2c_addr_lcd();


#endif /* LCD_I2C_H_ */