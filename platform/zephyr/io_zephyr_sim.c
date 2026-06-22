#include <dephy_industrial_io/zephyr_sim.h>

#include <string.h>

#include <zephyr/kernel.h>

#ifndef DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS
#define DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS 128
#endif

typedef struct {
    int32_t value;
    int32_t stuck_value;
    int32_t noise_span;
    uint32_t rng;
    uint8_t fault;
    uint8_t stuck;
} sim_channel_t;

static sim_channel_t g_channels[DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS];
static struct k_mutex g_lock;
static int g_lock_ready;

static void ensure_lock(void)
{
    if (!g_lock_ready) {
        k_mutex_init(&g_lock);
        g_lock_ready = 1;
    }
}

static int valid_channel(uint16_t driver_channel)
{
    return driver_channel < DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS;
}

static int32_t next_noise(sim_channel_t *channel)
{
    int32_t width;

    if (channel->noise_span <= 0) {
        return 0;
    }

    channel->rng = channel->rng * 1103515245u + 12345u;
    width = channel->noise_span * 2 + 1;
    return (int32_t)((channel->rng >> 16) % (uint32_t)width) - channel->noise_span;
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

    sim_channel_t *channel;

    if (!raw_value || !valid_channel(driver_channel)) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    channel = &g_channels[driver_channel];
    if (channel->fault) {
        k_mutex_unlock(&g_lock);
        return -1;
    }
    *raw_value = channel->stuck ? channel->stuck_value : channel->value;
    *raw_value += next_noise(channel);
    k_mutex_unlock(&g_lock);
    return 0;
}

static int sim_write_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value)
{
    (void)type;

    if (!valid_channel(driver_channel)) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    g_channels[driver_channel].value = raw_value;
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
    size_t i;

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    memset(g_channels, 0, sizeof(g_channels));
    for (i = 0; i < DEPHY_IO_ZEPHYR_SIM_MAX_CHANNELS; ++i) {
        g_channels[i].rng = 0x12340000u + (uint32_t)i;
    }
    k_mutex_unlock(&g_lock);
}

int dephy_io_zephyr_sim_set_raw(uint16_t driver_channel, int32_t raw_value)
{
    return sim_write_raw(driver_channel, DEPHY_IO_DI, raw_value);
}

int dephy_io_zephyr_sim_get_raw(uint16_t driver_channel, int32_t *raw_value)
{
    if (!raw_value || !valid_channel(driver_channel)) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    *raw_value = g_channels[driver_channel].value;
    k_mutex_unlock(&g_lock);
    return 0;
}

int dephy_io_zephyr_sim_set_fault(uint16_t driver_channel, int enabled)
{
    if (!valid_channel(driver_channel)) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    g_channels[driver_channel].fault = enabled ? 1u : 0u;
    k_mutex_unlock(&g_lock);
    return 0;
}

int dephy_io_zephyr_sim_set_stuck(uint16_t driver_channel, int enabled, int32_t raw_value)
{
    if (!valid_channel(driver_channel)) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    g_channels[driver_channel].stuck = enabled ? 1u : 0u;
    g_channels[driver_channel].stuck_value = raw_value;
    k_mutex_unlock(&g_lock);
    return 0;
}

int dephy_io_zephyr_sim_set_noise(uint16_t driver_channel, int32_t raw_span)
{
    if (!valid_channel(driver_channel) || raw_span < 0) {
        return -1;
    }

    ensure_lock();
    k_mutex_lock(&g_lock, K_FOREVER);
    g_channels[driver_channel].noise_span = raw_span;
    k_mutex_unlock(&g_lock);
    return 0;
}
