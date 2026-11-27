#include "fsm_automatic.h"
#include "global.h"
#include "software_timer.h"

// Hang so giao tiep voi module den va nut nhan cua TV2
#define LIGHT_OFF       0
#define LIGHT_GREEN     1
#define LIGHT_YELLOW    2
#define LIGHT_RED       3

#define BTN_MODE        0
#define BTN_RESET       4

// Khai bao weak de ca nhom Build 0 errors trong luc cho TV2 merge code
__attribute__((weak)) void traffic_set_way1(uint8_t color) {}
__attribute__((weak)) void traffic_set_way2(uint8_t color) {}
__attribute__((weak)) uint8_t is_button_pressed(uint8_t index) { return 0; }

// Cap nhat bo dem nguoc moi 1 giay de hien thi LCD
static void update_countdown_1s(void) {
    if (isTimerExpired(TIMER_1S)) {
        setTimer(TIMER_1S, 1000);
        if (counter_way1 > 0) counter_way1--;
        if (counter_way2 > 0) counter_way2--;
    }
}

// Kiem tra nut chuyen che do va nut reset mac dinh trong che do AUTO
static void check_auto_buttons(void) {
    if (is_button_pressed(BTN_MODE)) {
        status = MAN_INIT;
    }
    if (is_button_pressed(BTN_RESET)) {
        time_red = 5;
        time_green = 3;
        time_yellow = 2;
        status = INIT;
    }
}

void fsm_automatic_run(void) {
    switch (status) {
        case INIT:
            if (time_red != (time_green + time_yellow)) {
                time_red = time_green + time_yellow;
            }

            status = AUTO_RED_GREEN;
            traffic_set_way1(LIGHT_RED);
            traffic_set_way2(LIGHT_GREEN);

            counter_way1 = time_red;
            counter_way2 = time_green;

            setTimer(TIMER_TRAFFIC, time_green * 1000);
            setTimer(TIMER_1S, 1000);
            break;

        case AUTO_RED_GREEN:
            update_countdown_1s();

            if (isTimerExpired(TIMER_TRAFFIC)) {
                status = AUTO_RED_YELLOW;
                traffic_set_way1(LIGHT_RED);
                traffic_set_way2(LIGHT_YELLOW);

                counter_way1 = time_yellow;
                counter_way2 = time_yellow;

                setTimer(TIMER_TRAFFIC, time_yellow * 1000);
                setTimer(TIMER_1S, 1000);
            }

            check_auto_buttons();
            break;

        case AUTO_RED_YELLOW:
            update_countdown_1s();

            if (isTimerExpired(TIMER_TRAFFIC)) {
                status = AUTO_GREEN_RED;
                traffic_set_way1(LIGHT_GREEN);
                traffic_set_way2(LIGHT_RED);

                counter_way1 = time_green;
                counter_way2 = time_red;

                setTimer(TIMER_TRAFFIC, time_green * 1000);
                setTimer(TIMER_1S, 1000);
            }

            check_auto_buttons();
            break;

        case AUTO_GREEN_RED:
            update_countdown_1s();

            if (isTimerExpired(TIMER_TRAFFIC)) {
                status = AUTO_YELLOW_RED;
                traffic_set_way1(LIGHT_YELLOW);
                traffic_set_way2(LIGHT_RED);

                counter_way1 = time_yellow;
                counter_way2 = time_yellow;

                setTimer(TIMER_TRAFFIC, time_yellow * 1000);
                setTimer(TIMER_1S, 1000);
            }

            check_auto_buttons();
            break;

        case AUTO_YELLOW_RED:
            update_countdown_1s();

            if (isTimerExpired(TIMER_TRAFFIC)) {
                status = AUTO_RED_GREEN;
                traffic_set_way1(LIGHT_RED);
                traffic_set_way2(LIGHT_GREEN);

                counter_way1 = time_red;
                counter_way2 = time_green;

                setTimer(TIMER_TRAFFIC, time_green * 1000);
                setTimer(TIMER_1S, 1000);
            }

            check_auto_buttons();
            break;

        default:
            break;
    }
}
