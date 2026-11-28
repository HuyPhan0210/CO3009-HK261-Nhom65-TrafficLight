#include "traffic_light.h"
#include "main.h"


// BANG TO HOP TIN HIEU (A, B) CHO MODULE DEN GIAO THONG 2 DAY

#define TL_OFF_A        GPIO_PIN_RESET
#define TL_OFF_B        GPIO_PIN_RESET

#define TL_GREEN_A      GPIO_PIN_SET
#define TL_GREEN_B      GPIO_PIN_RESET

#define TL_YELLOW_A     GPIO_PIN_RESET
#define TL_YELLOW_B     GPIO_PIN_SET

#define TL_RED_A        GPIO_PIN_SET
#define TL_RED_B        GPIO_PIN_SET


static void traffic_write_pins(GPIO_TypeDef *port_a, uint16_t pin_a,
                               GPIO_TypeDef *port_b, uint16_t pin_b,
                               uint8_t color) {
    GPIO_PinState state_a = TL_OFF_A;
    GPIO_PinState state_b = TL_OFF_B;

    switch (color) {
        case LIGHT_GREEN:
            state_a = TL_GREEN_A;
            state_b = TL_GREEN_B;
            break;

        case LIGHT_YELLOW:
            state_a = TL_YELLOW_A;
            state_b = TL_YELLOW_B;
            break;

        case LIGHT_RED:
            state_a = TL_RED_A;
            state_b = TL_RED_B;
            break;

        case LIGHT_OFF:
        default:
            state_a = TL_OFF_A;
            state_b = TL_OFF_B;
            break;
    }

    HAL_GPIO_WritePin(port_a, pin_a, state_a);
    HAL_GPIO_WritePin(port_b, pin_b, state_b);
}

void traffic_set_way1(uint8_t color) {
    traffic_write_pins(TL1_A_GPIO_Port, TL1_A_Pin,
                       TL1_B_GPIO_Port, TL1_B_Pin,
                       color);
}

void traffic_set_way2(uint8_t color) {
    traffic_write_pins(TL2_A_GPIO_Port, TL2_A_Pin,
                       TL2_B_GPIO_Port, TL2_B_Pin,
                       color);
}

void traffic_clear_all(void) {
    traffic_set_way1(LIGHT_OFF);
    traffic_set_way2(LIGHT_OFF);
}
