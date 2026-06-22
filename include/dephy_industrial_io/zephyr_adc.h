#ifndef DEPHY_INDUSTRIAL_IO_ZEPHYR_ADC_H
#define DEPHY_INDUSTRIAL_IO_ZEPHYR_ADC_H

#include <stdint.h>

#include <zephyr/drivers/adc.h>

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

const dephy_io_driver_t *dephy_io_zephyr_adc_driver(void);
void dephy_io_zephyr_adc_reset(void);
int dephy_io_zephyr_adc_configure(uint16_t driver_channel,
                                  const struct adc_dt_spec *spec);
int dephy_io_zephyr_adc_raw_to_mv(const struct adc_dt_spec *spec,
                                  int32_t raw,
                                  int32_t *mv);
int dephy_io_zephyr_adc_mv_to_0_10v_x1000(int32_t mv);
int dephy_io_zephyr_adc_mv_to_4_20ma_x1000(int32_t mv,
                                           int32_t sense_ohms_x1000);

#ifdef __cplusplus
}
#endif

#endif

