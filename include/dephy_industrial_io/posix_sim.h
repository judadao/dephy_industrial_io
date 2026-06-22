#ifndef DEPHY_INDUSTRIAL_IO_POSIX_SIM_H
#define DEPHY_INDUSTRIAL_IO_POSIX_SIM_H

#include <stdint.h>

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

const dephy_io_driver_t *dephy_io_posix_sim_driver(void);
void dephy_io_posix_sim_reset(void);
int dephy_io_posix_sim_set_raw(uint16_t driver_channel, int32_t raw_value);
int dephy_io_posix_sim_get_raw(uint16_t driver_channel, int32_t *raw_value);
int dephy_io_posix_sim_set_fault(uint16_t driver_channel, int enabled);
int dephy_io_posix_sim_set_stuck(uint16_t driver_channel, int enabled, int32_t raw_value);
int dephy_io_posix_sim_set_noise(uint16_t driver_channel, int32_t raw_span);
void dephy_io_posix_sim_advance_ms(uint32_t delta_ms);

#ifdef __cplusplus
}
#endif

#endif
