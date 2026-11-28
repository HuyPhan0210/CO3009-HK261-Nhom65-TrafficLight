/*
 * dht20.c - doc cam bien DHT20 qua I2C1 (PB8 SCL, PB9 SDA)
 *
 * Moi chu ky 2 s:
 *   - Lay 5 mau, cach nhau 100 ms 
 *   - Mau nhieu thi bi bo qua, can it nhat 3 mau tot
 *   - Mau ngoai pham vi / khong du mau tot -> bao loi
 *   - Cac mau tot on dinh -> lay trung binh
 *   - Khong on dinh -> giu gia tri cu
 *
 * Chong loi I2C:
 *   - Moi lenh I2C goi i2c_bus_check(): loi 3 lan lien tiep thi reset khoi I2C
 *   - Dang bao loi thi moi chu ky kiem tra lai cam bien (vd rut day roi cam lai)
 */
#include "dht20.h"

extern I2C_HandleTypeDef hi2c1;

#define NUM_SAMPLES	5  	// so lan doc moi chu ky
#define MIN_GOOD   	3	// can it nhat 3 mau tot
#define CYCLE_STEPS 20  // 20 x 100 ms = 2 s cap nhat 1 lan

static float   temperature  = 0;
static float   humidity     = 0;
static uint8_t sensor_error = 1;

static int   step = 0; // buoc hien tai trong chu ky 0..19
static int   good_count; // so mau tot
static int   out_of_range; // 1 = co mau ngoai pham vi
static float t_sum, h_sum;
static float t_min, t_max, h_min, h_max;

static int i2c_error_count = 0;

static void i2c_bus_recover(void)
{
    hi2c1.Instance->CR1 |= I2C_CR1_SWRST;    // reset mem khoi I2C
    hi2c1.Instance->CR1 &= ~I2C_CR1_SWRST;
    HAL_I2C_DeInit(&hi2c1);
    HAL_I2C_Init(&hi2c1);
}

void i2c_bus_check(HAL_StatusTypeDef st)
{
    if (st == HAL_OK) {
        i2c_error_count = 0;
        return;
    }
    i2c_error_count++;
    if (i2c_error_count >= 3) {
        i2c_bus_recover();
        i2c_error_count = 0;
    }
}

static HAL_StatusTypeDef dht20_write(uint8_t *buf, uint16_t len)
{
    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, buf, len, I2C_TIMEOUT);
    i2c_bus_check(st);
    return st;
}

static HAL_StatusTypeDef dht20_read(uint8_t *buf, uint16_t len)
{
    HAL_StatusTypeDef st = HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, buf, len, I2C_TIMEOUT);
    i2c_bus_check(st);
    return st;
}

static uint8_t dht20_crc8(uint8_t *data, int len)
{
    uint8_t crc = 0xFF;
    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & 0x80) crc = (crc << 1) ^ 0x31;
            else            crc = crc << 1;
        }
    }
    return crc;
}

static void dht20_reset_reg(uint8_t reg)
{
    uint8_t buf[3] = { reg, 0x00, 0x00 };
    dht20_write(buf, 3);
    HAL_Delay(5);
    dht20_read(buf, 3);
    HAL_Delay(10);
    buf[0] = 0xB0 | reg;
    dht20_write(buf, 3);
}

static void dht20_check_status(void)
{
    uint8_t status_byte = 0;
    if (dht20_read(&status_byte, 1) != HAL_OK) return;
    if ((status_byte & 0x18) != 0x18) {
        dht20_reset_reg(0x1B);
        dht20_reset_reg(0x1C);
        dht20_reset_reg(0x1E);
    }
}

void dht20_init(void)
{
    HAL_Delay(100); // cho cam bien khoi dong
    dht20_check_status();
    sensor_error = 1;
}

/* Ra lenh do: gui 0xAC 0x33 0x00 */
static void dht20_start_measure(void)
{
    uint8_t cmd[3] = { 0xAC, 0x33, 0x00 };
    dht20_write(cmd, 3);
}

/* Doc 7 byte ket qua. Tra ve 1 neu mau tot */
static int dht20_read_sample(float *t, float *h)
{
    uint8_t d[7];
    if (dht20_read(d, 7) != HAL_OK) return 0;
    if (d[0] & 0x80) return 0;                      
    if (dht20_crc8(d, 6) != d[6]) return 0; // bi nhieu

    uint32_t raw_h = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
    uint32_t raw_t = (((uint32_t)d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];

    *h = raw_h * 100.0f / 1048576.0f;
    *t = raw_t * 200.0f / 1048576.0f - 50.0f;
    return 1;
}

/* Xu ly cac mau cua 1 chu ky */
static void dht20_process(void)
{
    if (out_of_range || good_count < MIN_GOOD) {
        sensor_error = 1;
    } else if (t_max - t_min < 0.5f && h_max - h_min < 3.0f) {
        temperature = t_sum / good_count; // trung binh cac mau tot
        humidity    = h_sum / good_count;
        sensor_error = 0;
    }
    // khong on dinh -> giu gia tri cu
}

void dht20_task(void)
{
    if (step == 0) {
        if (sensor_error) dht20_check_status();

        good_count = 0;
        out_of_range = 0;
        t_sum = 0;  h_sum = 0;
        t_min = 1000;  t_max = -1000;
        h_min = 1000;  h_max = -1000;
        dht20_start_measure();
    } else if (step <= NUM_SAMPLES) {
        float t, h;
        if (dht20_read_sample(&t, &h)) {
            if (t < -40 || t > 80 || h < 0 || h > 100) out_of_range = 1;
            t_sum += t;  h_sum += h;
            if (t < t_min) t_min = t;
            if (t > t_max) t_max = t;
            if (h < h_min) h_min = h;
            if (h > h_max) h_max = h;
            good_count++;
        }
        // mau hong thi bo qua

        if (step < NUM_SAMPLES) dht20_start_measure();   // do mau tiep theo
        else dht20_process(); // du 5 lan doc
    }

    step++;
    if (step >= CYCLE_STEPS) step = 0;
}

float   dht20_get_temperature(void) { return temperature; }
float   dht20_get_humidity(void)    { return humidity; }
uint8_t dht20_is_error(void)        { return sensor_error; }
