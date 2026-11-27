#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>

// Trang thai he thong chung cho 3 che do
#define INIT                0

// Che do Tu dong (TV1)
#define AUTO_RED_GREEN      1
#define AUTO_RED_YELLOW     2
#define AUTO_GREEN_RED      3
#define AUTO_YELLOW_RED     4

// Che do Chinh tay (TV2)
#define MAN_INIT            10
#define MAN_RED_GREEN       11
#define MAN_RED_YELLOW      12
#define MAN_GREEN_RED       13
#define MAN_YELLOW_RED      14

// Che do Tinh chinh thoi gian (TV3)
#define TUN_INIT            20
#define TUN_RED             21
#define TUN_YELLOW          22
#define TUN_GREEN           23

// Bien trang thai hien tai cua toan he thong
extern int status;

// Thoi gian cau hinh cho tung loai den (don vi: giay)
extern uint8_t time_red;
extern uint8_t time_green;
extern uint8_t time_yellow;

// Bien dem nguoc hien thi len LCD cho 2 tuyen (don vi: giay)
extern uint8_t counter_way1;
extern uint8_t counter_way2;

#endif /* INC_GLOBAL_H_ */
