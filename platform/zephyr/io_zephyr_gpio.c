#include <dephy_industrial_io/zephyr_gpio.h>

#include <string.h>

#include <zephyr/kernel.h>

#ifndef DEPHY_IO_ZEPHYR_GPIO_MAX_CHANNELS
#define DEPHY_IO_ZEPHYR_GPIO_MAX_CHANNELS 64
#endif

typedef struct {
    struct gpio_dt_spec spec;
    gpio_flags_t flags;
    uint8_t configured;
} gpio_channel_t;

static gpio_channel_t g_channels[DEPHY_IO_ZEPHYR_GPIO_MAX_CHANNELS];

static int zephyr_gpio_init(const dephy_io_channel_config_t *channels, size_t count)
{
    size_t i;

    for (i = 0; i < count; ++i) {
        gpio_channel_t *channel;
        gpio_flags_t direction;

        if (channels[i].driver_channel >= DEPHY_IO_ZEPHYR_GPIO_MAX_CHANNELS) {
            return -1;
        }

        channel = &g_channels[channels[i].driver_channel];
        if (!channel->configured || !gpio_is_ready_dt(&channel->spec)) {
            return -1;
        }

        direction = (channels[i].type == DEPHY_IO_DO || channels[i].type == DEPHY_IO_AO) ?
            GPIO_OUTPUT_INACTIVE : GPIO_INPUT;
        if (gpio_pin_configure_dt(&channel->spec, direction | channel->flags) != 0) {
            return -1;
        }
    }

    return 0;
}

static int zephyr_gpio_read_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t *raw_value)
{
    int value;

    if (!raw_value || driver_channel >= DEPHY_IO_ZEPHYR_GPIO_MAX_CHANNELS ||
        !g_channels[driver_channel].configured ||
        (type != DEPHY_IO_DI && type != DEPHY_IO_DO)) {
        return -1;
    }

    value = gpio_pin_get_dt(&g_channels[driver_channel].spec);
    if (value < 0) {
        return -1;
    }

    *raw_value = value;
    return 0;
}

static int zephyr_gpio_write_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value)
{
    if (driver_channel >= DEPHY_IO_ZEPHYR_GPIO_MAX_CHANNELS ||
        !g_channels[driver_channel].configured ||
        (type != DEPHY_IO_DO && type != DEPHY_IO_AO)) {
        return -1;
    }

    return gpio_pin_set_dt(&g_channels[driver_channel].spec, raw_value ? 1 : 0);
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

void dephy_io_zephyr_gpio_reset(void)
{
    memset(g_channels, 0, sizeof(g_channels));
}

int dephy_io_zephyr_gpio_configure(uint16_t driver_channel,
                                   const struct gpio_dt_spec *spec,
                                   gpio_flags_t flags)
{
    if (!spec || driver_channel >= DEPHY_IO_ZEPHYR_GPIO_MAX_CHANNELS) {
        return -1;
    }

    g_channels[driver_channel].spec = *spec;
    g_channels[driver_channel].flags = flags;
    g_channels[driver_channel].configured = 1;
    return 0;
}
