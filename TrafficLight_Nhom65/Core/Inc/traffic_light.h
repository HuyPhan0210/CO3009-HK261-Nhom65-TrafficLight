#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include <stdint.h>

// Quy uoc ma mau den giao thong
#define LIGHT_OFF       0
#define LIGHT_GREEN     1
#define LIGHT_YELLOW    2
#define LIGHT_RED       3

/**
 * @brief Dat trang thai mau den cho tuyen duong 1 (TL1_A, TL1_B).
 * @param color: LIGHT_OFF, LIGHT_GREEN, LIGHT_YELLOW, hoac LIGHT_RED.
 */
void traffic_set_way1(uint8_t color);

/**
 * @brief Dat trang thai mau den cho tuyen duong 2 (TL2_A, TL2_B).
 * @param color: LIGHT_OFF, LIGHT_GREEN, LIGHT_YELLOW, hoac LIGHT_RED.
 */
void traffic_set_way2(uint8_t color);

/**
 * @brief Tat ca 2 tuyen den (tien dung khi khoi tao hoac nhap nhay).
 */
void traffic_clear_all(void);

#endif /* INC_TRAFFIC_LIGHT_H_ */
