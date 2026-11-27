#include "button.h"
#include "main.h"


 // CAU HINH MUC LOGIC KHI NHAN NUT (ACTIVE-LOW / ACTIVE-HIGH)

#define BTN_EXT_PRESSED_STATE   GPIO_PIN_RESET  // 4 nut ngoai: BTN_1 .. BTN_4
#define BTN_U_PRESSED_STATE     GPIO_PIN_RESET  // Nut USER tren mach: BTN_U (PC13)

// Thoi gian tinh theo chu ky goi button_reading() = 10 ms
#define LONG_PRESS_TICKS        100             // 100 * 10 ms = 1000 ms (1 giay)
#define LONG_REPEAT_TICKS       50              // 50  * 10 ms = 500 ms  (lap lai moi 0.5 giay)

// Bang anh xa chi so 0..4 sang Port, Pin va muc logic khi nhan
static GPIO_TypeDef * const btn_ports[NUM_OF_BUTTONS] = {
    BTN_1_GPIO_Port,
    BTN_2_GPIO_Port,
    BTN_3_GPIO_Port,
    BTN_4_GPIO_Port,
    BTN_U_GPIO_Port
};

static const uint16_t btn_pins[NUM_OF_BUTTONS] = {
    BTN_1_Pin,
    BTN_2_Pin,
    BTN_3_Pin,
    BTN_4_Pin,
    BTN_U_Pin
};

static const GPIO_PinState btn_pressed_state[NUM_OF_BUTTONS] = {
    BTN_EXT_PRESSED_STATE,
    BTN_EXT_PRESSED_STATE,
    BTN_EXT_PRESSED_STATE,
    BTN_EXT_PRESSED_STATE,
    BTN_U_PRESSED_STATE
};

// 3 thanh ghi dich de chong doi (luu 3 lan doc gan nhat: 0 = tha, 1 = nhan)
static uint8_t key_reg0[NUM_OF_BUTTONS] = {0};
static uint8_t key_reg1[NUM_OF_BUTTONS] = {0};
static uint8_t key_reg2[NUM_OF_BUTTONS] = {0};

// Trang thai on dinh truoc do (dung de phat hien thoi diem chuyen 0 -> 1)
static uint8_t key_reg3[NUM_OF_BUTTONS] = {0};

// Bo dem nhan giu va cac co bao su kien cho FSM
static uint16_t hold_timer[NUM_OF_BUTTONS] = {0};
static uint8_t button_flag[NUM_OF_BUTTONS] = {0};
static uint8_t button_long_flag[NUM_OF_BUTTONS] = {0};

void button_reading(void) {
    for (uint8_t i = 0; i < NUM_OF_BUTTONS; i++) {
        // Buoc 1: Dich chuyen 3 mau doc gan nhat
        key_reg0[i] = key_reg1[i];
        key_reg1[i] = key_reg2[i];

        // Doc chan GPIO va quy doi ve chuan: 1 = dang nhan, 0 = dang tha
        if (HAL_GPIO_ReadPin(btn_ports[i], btn_pins[i]) == btn_pressed_state[i]) {
            key_reg2[i] = 1;
        } else {
            key_reg2[i] = 0;
        }

        // Buoc 2: Chi xu ly khi ca 3 lan doc lien tiep giong het nhau
        if ((key_reg0[i] == key_reg1[i]) && (key_reg1[i] == key_reg2[i])) {
            if (key_reg3[i] != key_reg2[i]) {
                // Trang thai on dinh vua thay doi (tu tha -> nhan, hoac nhan -> tha)
                key_reg3[i] = key_reg2[i];

                if (key_reg2[i] == 1) {
                    // Phat hien canh nhan (0 -> 1)
                    button_flag[i] = 1;
                    hold_timer[i] = LONG_PRESS_TICKS;
                }
            } else {
                // Trang thai khong doi (van dang tha hoac van dang giu)
                if (key_reg2[i] == 1) {
                    if (hold_timer[i] > 0) {
                        hold_timer[i]--;
                        if (hold_timer[i] == 0) {
                            button_long_flag[i] = 1;
                            hold_timer[i] = LONG_REPEAT_TICKS;
                        }
                    }
                }
            }
        }
    }
}

uint8_t is_button_pressed(uint8_t index) {
    if (index >= NUM_OF_BUTTONS) return 0;

    if (button_flag[index] == 1) {
        button_flag[index] = 0;
        return 1;
    }
    return 0;
}

uint8_t is_button_long_pressed(uint8_t index) {
    if (index >= NUM_OF_BUTTONS) return 0;

    if (button_long_flag[index] == 1) {
        button_long_flag[index] = 0;
        return 1;
    }
    return 0;
}
