#include "fsm_tuning.h"
#include "global.h"
#include "button.h"
#include "traffic_light.h"
#include "software_timer.h"

// Gioi han thoi gian (giay). Rang buoc: RED = GREEN + YELLOW
#define GREEN_MIN             1
#define GREEN_MAX             90
#define YELLOW_MIN            1
#define YELLOW_MAX            9
#define RED_MIN               2
#define RED_MAX               99
#define BLINK_HALF_PERIOD_MS  500   // 500 ms sang + 500 ms tat = 1 Hz

static uint8_t tmp_red    = 5;
static uint8_t tmp_yellow = 2;
static uint8_t tmp_green  = 3;
static uint8_t blink_on   = 1;

uint8_t fsm_tuning_get_red(void)	{ return tmp_red; }
uint8_t fsm_tuning_get_yellow(void)	{ return tmp_yellow; }
uint8_t fsm_tuning_get_green(void)	{ return tmp_green; }

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

static void handle_tuning_state(uint8_t color,
                                uint8_t *v1, uint8_t min1, uint8_t max1,
                                uint8_t *v2, uint8_t min2, uint8_t max2,
                                int next_status, uint8_t next_color)
{
    update_tuning_blink(color);

    if (is_button_pressed(BTN_UP) || is_button_long_pressed(BTN_UP)) {
        if (*v1 < max1 && *v2 < max2) {
            (*v1)++;
            (*v2)++;
        }
    }
    if (is_button_pressed(BTN_DOWN) || is_button_long_pressed(BTN_DOWN)) {
        if (*v1 > min1 && *v2 > min2) {
            (*v1)--;
            (*v2)--;
        }
    }

    if (is_button_pressed(BTN_SELECT)) {
        status = next_status;
        reset_blink_cycle(next_color);
    } else if (is_button_pressed(BTN_MODE)) {
        status = INIT;
    } else if (is_button_pressed(BTN_CONFIRM)) {
        time_red    = tmp_red;
        time_yellow = tmp_yellow;
        time_green  = tmp_green;
        status      = INIT;
    }
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
			(void)is_button_long_pressed(BTN_UP);
			(void)is_button_long_pressed(BTN_DOWN);

            status = TUN_RED;
            reset_blink_cycle(LIGHT_RED);
            break;

        case TUN_RED:
        	handle_tuning_state(LIGHT_RED,
								&tmp_red, RED_MIN, RED_MAX,
								&tmp_green, GREEN_MIN, GREEN_MAX,
								TUN_YELLOW, LIGHT_YELLOW);
            break;

        case TUN_YELLOW:
        	handle_tuning_state(LIGHT_YELLOW,
								&tmp_yellow, YELLOW_MIN, YELLOW_MAX,
								&tmp_red, RED_MIN, RED_MAX,
								TUN_GREEN, LIGHT_GREEN);
            break;

        case TUN_GREEN:
        	handle_tuning_state(LIGHT_GREEN,
								&tmp_green, GREEN_MIN, GREEN_MAX,
								&tmp_red, RED_MIN, RED_MAX,
								TUN_RED, LIGHT_RED);
            break;

        default:
            break;
    }
}
