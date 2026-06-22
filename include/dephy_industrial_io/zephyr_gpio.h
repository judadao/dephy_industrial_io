#ifndef DEPHY_INDUSTRIAL_IO_ZEPHYR_GPIO_H
#define DEPHY_INDUSTRIAL_IO_ZEPHYR_GPIO_H

#include <stdint.h>

#include <zephyr/drivers/gpio.h>

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

const dephy_io_driver_t *dephy_io_zephyr_gpio_driver(void);
void dephy_io_zephyr_gpio_reset(void);
int dephy_io_zephyr_gpio_configure(uint16_t driver_channel,
                                   const struct gpio_dt_spec *spec,
                                   gpio_flags_t flags);

#ifdef __cplusplus
}
#endif

#endif
