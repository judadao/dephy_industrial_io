#ifndef DEPHY_INDUSTRIAL_IO_ZEPHYR_SIM_H
#define DEPHY_INDUSTRIAL_IO_ZEPHYR_SIM_H

#include <stdint.h>

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

const dephy_io_driver_t *dephy_io_zephyr_sim_driver(void);
void dephy_io_zephyr_sim_reset(void);
int dephy_io_zephyr_sim_set_raw(uint16_t driver_channel, int32_t raw_value);
int dephy_io_zephyr_sim_get_raw(uint16_t driver_channel, int32_t *raw_value);

#ifdef __cplusplus
}
#endif

#endif

