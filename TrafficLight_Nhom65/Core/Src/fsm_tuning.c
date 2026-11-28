#include "fsm_tuning.h"
#include "global.h"
#include "button.h"
#include "traffic_light.h"
#include "software_timer.h"

#define BLINK_HALF_PERIOD_MS  500	// 500 ms sang + 500 ms tat = 1 Hz

static uint8_t tmp_red    = 5;
static uint8_t tmp_yellow = 2;
static uint8_t tmp_green  = 3;

static uint8_t blink_on   = 1;		// 1 = dang bat den, 0 = dang tat den

uint8_t fsm_tuning_check_time(uint8_t red, uint8_t yellow, uint8_t green)
{
    return (red == (uint8_t)(yellow + green))
        && (red >= RED_MIN && red <= RED_MAX)
        && (yellow >= YELLOW_MIN && yellow <= YELLOW_MAX)
        && (green >= GREEN_MIN && green <= GREEN_MAX);
}

uint8_t fsm_tuning_get_red(void)          { return tmp_red; }
uint8_t fsm_tuning_get_yellow(void)       { return tmp_yellow; }
uint8_t fsm_tuning_get_green(void)        { return tmp_green; }

// Khoi dong lai nhip chop (bat sang ngay lap tuc khi vua chon den)
static void reset_blink_cycle(uint8_t color)
{
    blink_on = 1;
    setTimer(TIMER_BLINK, BLINK_HALF_PERIOD_MS);
    traffic_set_way1(color);
    traffic_set_way2(color);
}

// Dao trang thai bat/tat moi 500 ms de tao tan so 1 Hz
static void update_tuning_blink(uint8_t color)
{
    if (isTimerExpired(TIMER_BLINK)) {
        setTimer(TIMER_BLINK, BLINK_HALF_PERIOD_MS);
        blink_on = !blink_on;
    }

    uint8_t output_color = blink_on ? color : LIGHT_OFF;
    traffic_set_way1(output_color);
    traffic_set_way2(output_color);
}

// Ap dung thoi gian da chinh va chuyen sang che do AUTO
static void save_and_exit_tuning(void)
{
    time_red    = tmp_red;
    time_yellow = tmp_yellow;
    time_green  = tmp_green;
    status      = INIT;
}

void fsm_tuning_run(void)
{
    switch (status) {
        case TUN_INIT:
            tmp_red    = time_red;
            tmp_yellow = time_yellow;
            tmp_green  = time_green;

            // Xoa co nut ton dong truoc khi bat dau chinh
			(void)is_button_pressed(BTN_SELECT);
			(void)is_button_pressed(BTN_CONFIRM);
			(void)is_button_pressed(BTN_UP);
			(void)is_button_pressed(BTN_DOWN);

            status = TUN_RED;
            reset_blink_cycle(LIGHT_RED);
            break;

        case TUN_RED:
        	update_tuning_blink(LIGHT_RED);

            // Tang Do -> Xanh tang theo
            if (is_button_pressed(BTN_UP) || is_button_long_pressed(BTN_UP)) {
                if (tmp_red < RED_MAX && tmp_green < GREEN_MAX) {
                    tmp_red++;
                    tmp_green++;
                }
            }
            // Giam Do -> Xanh giam theo
            if (is_button_pressed(BTN_DOWN) || is_button_long_pressed(BTN_DOWN)) {
                if (tmp_red > RED_MIN && tmp_green > GREEN_MIN) {
                    tmp_red--;
                    tmp_green--;
                }
            }

            if (is_button_pressed(BTN_SELECT)) {
                status = TUN_YELLOW;
                reset_blink_cycle(LIGHT_YELLOW);
            } else if (is_button_pressed(BTN_MODE)) {
                status = INIT;
            } else if (is_button_pressed(BTN_CONFIRM)) {
            	save_and_exit_tuning();
            }
            break;

        case TUN_YELLOW:
        	update_tuning_blink(LIGHT_YELLOW);

            // Tang Vang -> Do tang theo
            if (is_button_pressed(BTN_UP) || is_button_long_pressed(BTN_UP)) {
                if (tmp_yellow < YELLOW_MAX && tmp_red < RED_MAX) {
                    tmp_yellow++;
                    tmp_red++;
                }
            }
            // Giam Vang -> Do giam theo
            if (is_button_pressed(BTN_DOWN) || is_button_long_pressed(BTN_DOWN)) {
                if (tmp_yellow > YELLOW_MIN && tmp_red > RED_MIN) {
                    tmp_yellow--;
                    tmp_red--;
                }
            }

            if (is_button_pressed(BTN_SELECT)) {
                status = TUN_GREEN;
                reset_blink_cycle(LIGHT_GREEN);
            } else if (is_button_pressed(BTN_MODE)) {
                status = INIT;
            } else if (is_button_pressed(BTN_CONFIRM)) {
            	save_and_exit_tuning();
            }
            break;

        case TUN_GREEN:
        	update_tuning_blink(LIGHT_GREEN);

            // Tang Xanh -> Do tang theo
            if (is_button_pressed(BTN_UP) || is_button_long_pressed(BTN_UP)) {
                if (tmp_green < GREEN_MAX && tmp_red < RED_MAX) {
                    tmp_green++;
                    tmp_red++;
                }
            }
            // Giam Xanh -> Do giam theo
            if (is_button_pressed(BTN_DOWN) || is_button_long_pressed(BTN_DOWN)) {
                if (tmp_green > GREEN_MIN && tmp_red > RED_MIN) {
                    tmp_green--;
                    tmp_red--;
                }
            }

            if (is_button_pressed(BTN_SELECT)) {
                status = TUN_RED;
                reset_blink_cycle(LIGHT_RED);
            } else if (is_button_pressed(BTN_MODE)) {
                status = INIT;
            } else if (is_button_pressed(BTN_CONFIRM)) {
                save_and_exit_tuning();
            }
            break;

        default:
            break;
    }
}
