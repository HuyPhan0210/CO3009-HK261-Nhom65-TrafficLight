#ifndef INC_LCD_H_
#define INC_LCD_H_

#define LCD_ADDR  (0x21 << 1)

void lcd_init(void);                              // goi 1 lan trong main(), truoc while(1)
void lcd_clear(void);
void lcd_goto(int row, int col);                  // row 0..1, col 0..15
void lcd_print(const char *str);
void lcd_print_line(int row, const char *str);    // in ca dong 16 ky tu
void lcd_display(void);                           // task scheduler, goi moi 50 ms

#endif /* INC_LCD_H_ */
