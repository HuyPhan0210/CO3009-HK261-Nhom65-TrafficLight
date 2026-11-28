#include "fsm_automatic.h"
#include "global.h"
#include "software_timer.h"
#include "button.h"
#include "traffic_light.h"

static void set_auto_phase(int next_status,
                           uint8_t light1, uint8_t light2,
                           uint8_t c1, uint8_t c2,
                           uint8_t duration_s)
{
    status = next_status;
    traffic_set_way1(light1);
    traffic_set_way2(light2);
    counter_way1 = c1;
    counter_way2 = c2;
    setTimer(TIMER_TRAFFIC, (uint32_t)duration_s * 1000);
    setTimer(TIMER_1S, 1000);
}

static void update_countdown_1s(void) {
    if (isTimerExpired(TIMER_1S)) {
        setTimer(TIMER_1S, 1000);
        if (counter_way1 > 0) counter_way1--;
        if (counter_way2 > 0) counter_way2--;
    }
}

void fsm_automatic_run(void) {
    switch (status) {
        case INIT:
        	time_red = time_green + time_yellow;
        	set_auto_phase(AUTO_RED_GREEN,
        				   LIGHT_RED, LIGHT_GREEN,
						   time_red, time_green,
						   time_green);
            break;

        case AUTO_RED_GREEN:
            update_countdown_1s();
            if (isTimerExpired(TIMER_TRAFFIC)) {
				set_auto_phase(AUTO_RED_YELLOW,
							   LIGHT_RED, LIGHT_YELLOW,
							   time_yellow, time_yellow,
							   time_yellow);
			}
            break;

        case AUTO_RED_YELLOW:
            update_countdown_1s();
            if (isTimerExpired(TIMER_TRAFFIC)) {
				set_auto_phase(AUTO_GREEN_RED,
						 	   LIGHT_GREEN, LIGHT_RED,
							   time_green, time_red,
							   time_green);
			}
            break;

        case AUTO_GREEN_RED:
            update_countdown_1s();
            if (isTimerExpired(TIMER_TRAFFIC)) {
				set_auto_phase(AUTO_YELLOW_RED,
							   LIGHT_YELLOW, LIGHT_RED,
							   time_yellow, time_yellow,
							   time_yellow);
			}
            break;

        case AUTO_YELLOW_RED:
            update_countdown_1s();
            if (isTimerExpired(TIMER_TRAFFIC)) {
				set_auto_phase(AUTO_RED_GREEN,
							   LIGHT_RED, LIGHT_GREEN,
							   time_red, time_green,
							   time_green);
			}
            break;

        default:
            return;
    }

    if (is_button_pressed(BTN_MODE))
		status = MAN_INIT;
}
