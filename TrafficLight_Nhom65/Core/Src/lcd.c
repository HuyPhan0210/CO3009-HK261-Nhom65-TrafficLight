#include "lcd.h"
#include "global.h"
#include "dht20.h"
#include "fsm_tuning.h"
#include <stdio.h>
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define LCD_BACKLIGHT  0x08
#define LCD_EN         0x04
#define LCD_RS         0x01
#define REINIT_PERIOD  50      // 50 lan x 200 ms = 10 s

static char screen[2][17];     // noi dung dang hien thi tren LCD
static int  lcd_error = 0;     // 1 = vua gui loi, can khoi tao lai
static int  reinit_counter = 0;

static void lcd_write(uint8_t *data, uint16_t len)
{
    if (lcd_error) return;  // dang loi -> cho khoi tao lai
    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data, len, I2C_TIMEOUT);
    i2c_bus_check(st);
    if (st != HAL_OK) lcd_error = 1;
}

/* Gui 1 byte theo 2 nua 4 bit, moi nua tao 1 xung EN */
static void lcd_send(uint8_t value, uint8_t rs)
{
    uint8_t high = (value & 0xF0) | LCD_BACKLIGHT | rs;
    uint8_t low  = ((value << 4) & 0xF0) | LCD_BACKLIGHT | rs;
    uint8_t data[4] = { high | LCD_EN, high, low | LCD_EN, low };
    lcd_write(data, 4);
}

static void lcd_send_cmd(uint8_t cmd)   { lcd_send(cmd, 0); }
static void lcd_send_data(uint8_t ch)   { lcd_send(ch, LCD_RS); }

static void lcd_send_nibble(uint8_t nibble)
{
    uint8_t value = (nibble << 4) | LCD_BACKLIGHT;
    uint8_t data[2] = { value | LCD_EN, value };
    lcd_write(data, 2);
}

/* Khoi tao LCD va ve lai noi dung da nho. Mat khoang 12 ms. */
static void lcd_reinit(void)
{
    lcd_error = 0;
    lcd_send_nibble(0x3); HAL_Delay(5);
    lcd_send_nibble(0x3); HAL_Delay(1);
    lcd_send_nibble(0x3); HAL_Delay(1);
    lcd_send_nibble(0x2); HAL_Delay(1);

    lcd_send_cmd(0x28);
    lcd_send_cmd(0x08); 
    lcd_clear();
    lcd_send_cmd(0x06);
    lcd_send_cmd(0x0C); 

    for (int row = 0; row < 2; row++) {
        lcd_goto(row, 0);
        lcd_print(screen[row]);
    }
}

void lcd_init(void)
{
    memset(screen, ' ', sizeof(screen));
    screen[0][16] = '\0';
    screen[1][16] = '\0';
    HAL_Delay(50); // cho LCD on dinh nguon
    lcd_reinit();
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_goto(int row, int col)
{
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    lcd_send_cmd(0x80 | (addr + col));
}

void lcd_print(const char *str)
{
    while (*str) lcd_send_data(*str++);
}
void lcd_print_line(int row, const char *str)
{
    char line[17];
    int i = 0;
    while (i < 16 && str[i] != '\0') { line[i] = str[i]; i++; }
    while (i < 16) line[i++] = ' ';
    line[16] = '\0';

    if (strcmp(line, screen[row]) == 0) return;   // khong doi
    strcpy(screen[row], line);
    lcd_goto(row, 0);
    lcd_print(line);
}

/* Doi so thuc sang chuoi 1 chu so thap phan, vd 28.46 -> "28.5" */
static void float_to_str(char *out, float x)
{
    int sign = (x < 0);
    if (sign) x = -x;
    int v = (int)(x * 10 + 0.5f);          // lam tron, doi ra so nguyen x10
    sprintf(out, "%s%d.%d", sign ? "-" : "", v / 10, v % 10);
}

static void show_temp_humi(void)
{
    char line[24], t[8], h[8];
    if (dht20_is_error()) {
        lcd_print_line(1, "Sensor error!");
        return;
    }
    float_to_str(t, dht20_get_temperature());
    float_to_str(h, dht20_get_humidity());
    sprintf(line, "T:%s%cC H:%s%%", t, 223, h);   // 223 = ky tu do
    lcd_print_line(1, line);
}

static void show_tuning(void)
{
    char line[24];
    uint8_t r = fsm_tuning_get_red();
    uint8_t y = fsm_tuning_get_yellow();
    uint8_t g = fsm_tuning_get_green();

    if (fsm_tuning_is_showing_error()) {
        lcd_print_line(0, "ERR: R != Y+G");
        sprintf(line, "R=%02d  Y+G=%02d", r, y + g);
        lcd_print_line(1, line);
        return;
    }

    if (status == TUN_RED)         sprintf(line, "TUNE RED     %02ds", r);
    else if (status == TUN_YELLOW) sprintf(line, "TUNE YELLOW  %02ds", y);
    else                           sprintf(line, "TUNE GREEN   %02ds", g);
    lcd_print_line(0, line);

    // dau '>' chi den dang chinh, OK/ERR cho biet bo gia tri co hop le khong
    sprintf(line, "%cR%02d%cY%02d%cG%02d %s",
            status == TUN_RED    ? '>' : ' ', r,
            status == TUN_YELLOW ? '>' : ' ', y,
            status == TUN_GREEN  ? '>' : ' ', g,
            fsm_tuning_check_time(r, y, g) ? " OK" : "ERR");
    lcd_print_line(1, line);
}

void lcd_display(void)
{
    char line[24];
    reinit_counter++;
    if (lcd_error || reinit_counter >= REINIT_PERIOD) {
        reinit_counter = 0;
        lcd_reinit();
    }

    if (status >= TUN_INIT && status <= TUN_GREEN) {
        if (status != TUN_INIT) show_tuning();
    } else if (status >= MAN_INIT && status <= MAN_YELLOW_RED) {
        lcd_print_line(0, "MODE: MANUAL");
        show_temp_humi();
    } else {
        sprintf(line, "AUTO W1:%02d W2:%02d", counter_way1, counter_way2);
        lcd_print_line(0, line);
        show_temp_humi();
    }
}
