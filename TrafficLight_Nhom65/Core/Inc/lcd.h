#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

// Dia chi LCD 7 bit. Code ESP32 cu dung 33 = 0x21.
// Neu LCD khong hien gi: quet I2C (Buoc 3.4 file huong dan) roi sua o day.
#define LCD_ADDR  (0x21 << 1)

void lcd_init(void);                              // goi 1 lan trong main(), truoc while(1)
void lcd_clear(void);
void lcd_goto(int row, int col);                  // row 0..1, col 0..15
void lcd_print(const char *str);
void lcd_print_line(int row, const char *str);    // in ca dong 16 ky tu

void lcd_display(void);                           // task scheduler, goi moi 200 ms

#endif /* INC_LCD_H_ */
