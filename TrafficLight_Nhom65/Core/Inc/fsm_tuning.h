#ifndef INC_FSM_TUNING_H_
#define INC_FSM_TUNING_H_

#include <stdint.h>

void    fsm_tuning_run(void); 		// task scheduler, goi moi 10 ms
uint8_t fsm_tuning_get_red(void);
uint8_t fsm_tuning_get_yellow(void);
uint8_t fsm_tuning_get_green(void);

#endif /* INC_FSM_TUNING_H_ */
