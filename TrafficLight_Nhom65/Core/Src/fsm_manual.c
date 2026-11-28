#include "fsm_manual.h"
#include "global.h"
#include "traffic_light.h"
#include "button.h"

static void handle_manual_state(uint8_t light1, uint8_t light2, int next_status)
{
    traffic_set_way1(light1);
    traffic_set_way2(light2);

    if (is_button_pressed(BTN_MODE)) {
        status = TUN_INIT;
    } else if (is_button_pressed(BTN_SELECT)) {
        status = next_status;
    }
}

void fsm_manual_run(void) {
    switch (status) {
        case MAN_INIT:
        	traffic_set_way1(LIGHT_RED);
			traffic_set_way2(LIGHT_GREEN);
			counter_way1 = 0;
			counter_way2 = 0;
			(void)is_button_pressed(BTN_SELECT);
			status = MAN_RED_GREEN;
			break;

        case MAN_RED_GREEN:
        	handle_manual_state(LIGHT_RED, LIGHT_GREEN, MAN_RED_YELLOW);
            break;

        case MAN_RED_YELLOW:
        	handle_manual_state(LIGHT_RED, LIGHT_YELLOW, MAN_GREEN_RED);
            break;

        case MAN_GREEN_RED:
        	handle_manual_state(LIGHT_GREEN, LIGHT_RED, MAN_YELLOW_RED);
            break;

        case MAN_YELLOW_RED:
        	handle_manual_state(LIGHT_YELLOW, LIGHT_RED, MAN_RED_GREEN);
            break;

        default:
            break;
    }
}
