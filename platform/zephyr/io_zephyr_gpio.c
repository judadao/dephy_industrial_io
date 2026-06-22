#include <dephy_industrial_io/industrial_io.h>

#include <zephyr/kernel.h>

static int zephyr_gpio_init(const dephy_io_channel_config_t *channels, size_t count)
{
    (void)channels;
    (void)count;
    return 0;
}

static int zephyr_gpio_read_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t *raw_value)
{
    (void)driver_channel;
    (void)type;
    (void)raw_value;
    return -1;
}

static int zephyr_gpio_write_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value)
{
    (void)driver_channel;
    (void)type;
    (void)raw_value;
    return -1;
}

static uint32_t zephyr_now_ms(void)
{
    return (uint32_t)k_uptime_get_32();
}

const dephy_io_driver_t *dephy_io_zephyr_gpio_driver(void)
{
    static const dephy_io_driver_t driver = {
        .init = zephyr_gpio_init,
        .read_raw = zephyr_gpio_read_raw,
        .write_raw = zephyr_gpio_write_raw,
        .now_ms = zephyr_now_ms,
    };

    return &driver;
}

