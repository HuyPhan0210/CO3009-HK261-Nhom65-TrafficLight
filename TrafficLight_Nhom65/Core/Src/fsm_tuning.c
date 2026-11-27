/*
 * Rang buoc: RED = GREEN + YELLOW (do nay = xanh + vang ).
 */
#include "fsm_tuning.h"
#include "global.h"

#define LIGHT_RED       3

#define BTN_MODE        0     
#define BTN_SEL         1     
#define BTN_INC         2     // +1 giay
#define BTN_DEC         3     // -1 giay
#define BTN_RESET       4

#define ERROR_SHOW_TIME 200   // 200 x 10 ms = 2 s
void traffic_set_way1(uint8_t color);
void traffic_set_way2(uint8_t color);
uint8_t is_button_pressed(uint8_t index);

__attribute__((weak)) uint8_t is_button_long_pressed(uint8_t index) { (void)index; return 0; }

static uint8_t new_red, new_yellow, new_green;   
static int error_counter = 0;  // > 0: dang hien thong bao loi

uint8_t fsm_tuning_check_time(uint8_t red, uint8_t yellow, uint8_t green)
{
    if (green < GREEN_MIN || green  > GREEN_MAX) return 0;
    if (yellow < YELLOW_MIN || yellow > YELLOW_MAX) return 0;
    if (red < RED_MIN || red > RED_MAX) return 0;
    if (red != green + yellow) return 0;
    return 1;
}

static uint8_t limit(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return (uint8_t)value;
}

static void change_value(int delta)
{
    if (status == TUN_RED) new_red = limit(new_red + delta, RED_MIN, RED_MAX);
    else if (status == TUN_YELLOW) new_yellow = limit(new_yellow + delta, YELLOW_MIN, YELLOW_MAX);
    else new_green = limit(new_green + delta, GREEN_MIN, GREEN_MAX);
}

void fsm_tuning_run(void)
{
    switch (status) {
    case TUN_INIT:
        // lay thoi gian hien tai ra de chinh
        new_red = limit(time_red, RED_MIN, RED_MAX);
        new_yellow = limit(time_yellow, YELLOW_MIN, YELLOW_MAX);
        new_green = limit(time_green, GREEN_MIN, GREEN_MAX);
        error_counter = 0;

        traffic_set_way1(LIGHT_RED); // an toan: ca 2 tuyen do
        traffic_set_way2(LIGHT_RED);
        status = TUN_RED;
        break;

    case TUN_RED:
    case TUN_YELLOW:
    case TUN_GREEN:
        if (error_counter > 0) {
            // dang bao loi: bo qua moi nut nhan
            for (uint8_t i = 0; i <= BTN_RESET; i++) is_button_pressed(i);
            error_counter--;
            break;
        }

        if (is_button_pressed(BTN_SEL)) {
            if (status == TUN_RED) status = TUN_YELLOW;
            else if (status == TUN_YELLOW) status = TUN_GREEN;
            else status = TUN_RED;
        }
        else if (is_button_pressed(BTN_INC) || is_button_long_pressed(BTN_INC)) {
            change_value(+1);
        }
        else if (is_button_pressed(BTN_DEC) || is_button_long_pressed(BTN_DEC)) {
            change_value(-1);
        }
        else if (is_button_pressed(BTN_MODE)) {
            if (fsm_tuning_check_time(new_red, new_yellow, new_green)) {
                time_red  = new_red;       // ap dung thoi gian moi
                time_yellow = new_yellow;
                time_green = new_green;
                status = INIT; // fsm_automatic bat dau lai tu dau
            } else {
                error_counter = ERROR_SHOW_TIME;   // khong ap dung
                status = TUN_RED;  // quay ve sua den do
            }
        }
        else if (is_button_pressed(BTN_RESET)) {
            status = INIT; // huy, giu thoi gian cu
        }
        break;

    default:
        break; // khong o che do chinh -> khong lam gi
    }
}

uint8_t fsm_tuning_get_red(void) { return new_red; }
uint8_t fsm_tuning_get_yellow(void) { return new_yellow; }
uint8_t fsm_tuning_get_green(void) { return new_green; }
uint8_t fsm_tuning_is_showing_error(void) { return error_counter > 0; }
