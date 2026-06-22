#include <dephy_industrial_io/posix_sim.h>

#include <string.h>

#ifndef DEPHY_IO_POSIX_SIM_MAX_CHANNELS
#define DEPHY_IO_POSIX_SIM_MAX_CHANNELS 128
#endif

static int32_t g_values[DEPHY_IO_POSIX_SIM_MAX_CHANNELS];
static uint32_t g_now_ms;

static int sim_init(const dephy_io_channel_config_t *channels, size_t count)
{
    (void)channels;
    (void)count;
    return 0;
}

static int sim_read_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t *raw_value)
{
    (void)type;

    if (!raw_value || driver_channel >= DEPHY_IO_POSIX_SIM_MAX_CHANNELS) {
        return -1;
    }

    *raw_value = g_values[driver_channel];
    return 0;
}

static int sim_write_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value)
{
    (void)type;

    if (driver_channel >= DEPHY_IO_POSIX_SIM_MAX_CHANNELS) {
        return -1;
    }

    g_values[driver_channel] = raw_value;
    return 0;
}

static uint32_t sim_now_ms(void)
{
    return g_now_ms;
}

const dephy_io_driver_t *dephy_io_posix_sim_driver(void)
{
    static const dephy_io_driver_t driver = {
        .init = sim_init,
        .read_raw = sim_read_raw,
        .write_raw = sim_write_raw,
        .now_ms = sim_now_ms,
    };

    return &driver;
}

void dephy_io_posix_sim_reset(void)
{
    memset(g_values, 0, sizeof(g_values));
    g_now_ms = 0;
}

int dephy_io_posix_sim_set_raw(uint16_t driver_channel, int32_t raw_value)
{
    if (driver_channel >= DEPHY_IO_POSIX_SIM_MAX_CHANNELS) {
        return -1;
    }

    g_values[driver_channel] = raw_value;
    return 0;
}

int dephy_io_posix_sim_get_raw(uint16_t driver_channel, int32_t *raw_value)
{
    if (!raw_value || driver_channel >= DEPHY_IO_POSIX_SIM_MAX_CHANNELS) {
        return -1;
    }

    *raw_value = g_values[driver_channel];
    return 0;
}

void dephy_io_posix_sim_advance_ms(uint32_t delta_ms)
{
    g_now_ms += delta_ms;
}

