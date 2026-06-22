#ifndef DEPHY_INDUSTRIAL_IO_ZEPHYR_GPIO_H
#define DEPHY_INDUSTRIAL_IO_ZEPHYR_GPIO_H

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

const dephy_io_driver_t *dephy_io_zephyr_gpio_driver(void);

#ifdef __cplusplus
}
#endif

#endif

