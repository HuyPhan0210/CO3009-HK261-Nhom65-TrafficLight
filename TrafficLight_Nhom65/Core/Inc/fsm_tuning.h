#ifndef INC_FSM_TUNING_H_
#define INC_FSM_TUNING_H_

#include <stdint.h>

// Gioi han thoi gian (giay). Rang buoc: RED = GREEN + YELLOW
#define GREEN_MIN    1
#define GREEN_MAX    90
#define YELLOW_MIN   1
#define YELLOW_MAX   9
#define RED_MIN      2
#define RED_MAX      99

void    fsm_tuning_run(void); // task scheduler, goi moi 10 ms
uint8_t fsm_tuning_check_time(uint8_t red, uint8_t yellow, uint8_t green); // 1 = hop le

// Cho LCD doc gia tri dang chinh
uint8_t fsm_tuning_get_red(void);
uint8_t fsm_tuning_get_yellow(void);
uint8_t fsm_tuning_get_green(void);
uint8_t fsm_tuning_is_showing_error(void); // 1 = dang hien thong bao loi

#endif /* INC_FSM_TUNING_H_ */
