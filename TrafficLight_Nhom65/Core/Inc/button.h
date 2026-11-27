#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>

#define NUM_OF_BUTTONS  5

// Quy uoc chi so 5 nut nhan (khop BTN_MODE = 0, BTN_RESET = 4)
#define BTN_MODE        0   // BTN_1 (PA8)  - Chuyen che do AUTO / MANUAL / TUNING
#define BTN_UP          1   // BTN_2 (PA9)  - Chuyen pha den (MANUAL) / Tang thoi gian (TUNING)
#define BTN_DOWN        2   // BTN_3 (PC7)  - Chuyen pha nguoc (MANUAL) / Giam hoac chon den (TUNING)
#define BTN_CONFIRM     3   // BTN_4 (PB6)  - Xac nhan / Luu cau hinh (TUNING)
#define BTN_RESET       4   // BTN_U (PC13) - Reset mac dinh (5s - 3s - 2s)

/**
 * @brief Doc va chong doi tat ca 5 nut nhan.
 */
void button_reading(void);

/**
 * @brief Kiem tra nut co vua duoc nhan don hay khong (tu dong xoa co sau khi doc).
 * @param index: Chi so nut (0..4) hoac macro BTN_MODE .. BTN_RESET.
 * @return 1 neu vua nhan, 0 neu khong.
 */
uint8_t is_button_pressed(uint8_t index);

/**
 * @brief Kiem tra nut co dang duoc nhan giu lau (>= 1 giay) hay khong.
 * @param index: Chi so nut (0..4).
 * @return 1 moi khi den nhip nhan giu, 0 neu khong.
 */
uint8_t is_button_long_pressed(uint8_t index);

#endif /* INC_BUTTON_H_ */
