#include "fsm_manual.h"
#include "global.h"
#include "traffic_light.h"
#include "button.h"
#include <stdint.h>

/**
 * @brief Kiem tra 2 nut thoat che do MANUAL (BTN_MODE va BTN_RESET).
 *        Chi duoc goi ben trong cac trang thai MAN_* de tranh cuop co nut
 *        cua che do AUTO va TUNING.
 * @return 1 neu da chuyen sang che do khac, 0 neu van o lai MANUAL.
 */
static uint8_t manual_check_exit(void) {
    // Nhan BTN_MODE -> chuyen sang che do tinh chinh thoi gian (TUNING - TV3)
    if (is_button_pressed(BTN_MODE)) {
        status = TUN_INIT;
        return 1;
    }

    // Nhan BTN_RESET -> khoi phuc thoi gian mac dinh (5s - 3s - 2s) va ve AUTO
    // LUU Y: Doi chieu ten 3 bien thoi gian va don vi (giay hay ms) voi Core/Inc/global.h
    if (is_button_pressed(BTN_RESET)) {
        time_red    = 5;
        time_green  = 3;
        time_yellow = 2;
        status = INIT;
        return 1;
    }

    return 0;
}

void fsm_manual_run(void) {
    switch (status) {
        case MAN_INIT:
            // Buoc 1: Dua he thong ve pha an toan chuan (Tuyen 1 DO - Tuyen 2 XANH)
            traffic_set_way1(LIGHT_RED);
            traffic_set_way2(LIGHT_GREEN);

            // Buoc 2: Xoa gia tri dem nguoc cu cua AUTO de LCD khong hien so thua
            counter_way1 = 0;
            counter_way2 = 0;

            // Buoc 3: Xoa cac co nut nhan ton dong tu che do truoc
            // (tranh truong hop vua vao MANUAL da bi nhay pha ngay)
            (void)is_button_pressed(BTN_UP);
            (void)is_button_pressed(BTN_DOWN);
            (void)is_button_pressed(BTN_CONFIRM);

            // Buoc 4: Chuyen sang trang thai giu den dau tien
            status = MAN_RED_GREEN;
            break;

        case MAN_RED_GREEN:
            traffic_set_way1(LIGHT_RED);
            traffic_set_way2(LIGHT_GREEN);

            if (manual_check_exit()) break;

            if (is_button_pressed(BTN_UP)) {
                status = MAN_RED_YELLOW;
            } else if (is_button_pressed(BTN_DOWN)) {
                status = MAN_YELLOW_RED;
            }
            break;

        case MAN_RED_YELLOW:
            traffic_set_way1(LIGHT_RED);
            traffic_set_way2(LIGHT_YELLOW);

            if (manual_check_exit()) break;

            if (is_button_pressed(BTN_UP)) {
                status = MAN_GREEN_RED;
            } else if (is_button_pressed(BTN_DOWN)) {
                status = MAN_RED_GREEN;
            }
            break;

        case MAN_GREEN_RED:
            traffic_set_way1(LIGHT_GREEN);
            traffic_set_way2(LIGHT_RED);

            if (manual_check_exit()) break;

            if (is_button_pressed(BTN_UP)) {
                status = MAN_YELLOW_RED;
            } else if (is_button_pressed(BTN_DOWN)) {
                status = MAN_RED_YELLOW;
            }
            break;

        case MAN_YELLOW_RED:
            traffic_set_way1(LIGHT_YELLOW);
            traffic_set_way2(LIGHT_RED);

            if (manual_check_exit()) break;

            if (is_button_pressed(BTN_UP)) {
                status = MAN_RED_GREEN;
            } else if (is_button_pressed(BTN_DOWN)) {
                status = MAN_GREEN_RED;
            }
            break;

        default:
            // Khong thuoc che do MANUAL (dang o AUTO hoac TUNING) -> bo qua
            break;
    }
}
