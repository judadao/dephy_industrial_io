#include <dephy_industrial_io/zephyr_sim.h>

#include <string.h>

#include <zephyr/kernel.h>

#ifndef DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS
#define DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS 128
#endif

static int32_t g_values[DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS];
static struct k_mutex g_lock;
static int g_lock_ready;

static void ensure_lock(void)
{
    if (!g_lock_ready) {
        k_mutex_init(&g_lock);
        g_lock_ready = 1;
    }
}

static int sim_init(const dephy_io_channel_config_t *channels, size_t count)
{
    (void)channels;
    (void)count;
    ensure_lock();
    return 0;
}

static int sim_read_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t *raw_value)
{
    (void)type;

    if (!raw_value || driver_channel >= DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    *raw_value = g_values[driver_channel];
    k_mutex_unlock(&g_lock);
    return 0;
}

static int sim_write_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value)
{
    (void)type;

    if (driver_channel >= DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    g_values[driver_channel] = raw_value;
    k_mutex_unlock(&g_lock);
    return 0;
}

static uint32_t sim_now_ms(void)
{
    return k_uptime_get_32();
}

const dephy_io_driver_t *dephy_io_zephyr_sim_driver(void)
{
    static const dephy_io_driver_t driver = {
        .init = sim_init,
        .read_raw = sim_read_raw,
        .write_raw = sim_write_raw,
        .now_ms = sim_now_ms,
    };

    ensure_lock();
    return &driver;
}

void dephy_io_zephyr_sim_reset(void)
{
    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    memset(g_values, 0, sizeof(g_values));
    k_mutex_unlock(&g_lock);
}

int dephy_io_zephyr_sim_set_raw(uint16_t driver_channel, int32_t raw_value)
{
    return sim_write_raw(driver_channel, DEPHY_IO_DI, raw_value);
}

int dephy_io_zephyr_sim_get_raw(uint16_t driver_channel, int32_t *raw_value)
{
    return sim_read_raw(driver_channel, DEPHY_IO_DI, raw_value);
}

