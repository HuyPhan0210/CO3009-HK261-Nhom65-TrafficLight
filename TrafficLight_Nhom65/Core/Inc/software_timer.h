#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

#define TIMER_CYCLE_MS  10  // Chu kỳ ngắt TIM2 = 10 ms

// Danh sách các timer dùng chung cho toàn dự án
#define TIMER_TRAFFIC   0   // Chuyển trạng thái pha đèn giao thông 2 chiều
#define TIMER_1S        1   // Đếm ngược mỗi 1 giây cho biến hiển thị LCD
#define TIMER_BLINK     2   // Nhấp nháy đèn LED ở chế độ Manual / Tuning
#define TIMER_LED       3   // Nháy LED_1 báo trạng thái hệ thống
#define MAX_TIMERS      4

void setTimer(uint8_t index, uint32_t duration_ms);
uint8_t isTimerExpired(uint8_t index);
void timerRun(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
