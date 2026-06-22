#include <dephy_industrial_io/zephyr_adc.h>

#include <string.h>

#include <zephyr/kernel.h>

#ifndef DEPHY_IO_ZEPHYR_ADC_MAX_CHANNELS
#define DEPHY_IO_ZEPHYR_ADC_MAX_CHANNELS 32
#endif

typedef struct {
    struct adc_dt_spec spec;
    int16_t sample;
    uint8_t configured;
} adc_channel_t;

static adc_channel_t g_channels[DEPHY_IO_ZEPHYR_ADC_MAX_CHANNELS];

static int zephyr_adc_init(const dephy_io_channel_config_t *channels, size_t count)
{
    size_t i;

    for (i = 0; i < count; ++i) {
        adc_channel_t *channel;

        if (channels[i].type != DEPHY_IO_AI ||
            channels[i].driver_channel >= DEPHY_IO_ZEPHYR_ADC_MAX_CHANNELS) {
            return -1;
        }

        channel = &g_channels[channels[i].driver_channel];
        if (!channel->configured || !adc_is_ready_dt(&channel->spec)) {
            return -1;
        }
        if (adc_channel_setup_dt(&channel->spec) != 0) {
            return -1;
        }
    }

    return 0;
}

static int zephyr_adc_read_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t *raw_value)
{
    adc_channel_t *channel;
    struct adc_sequence sequence;

    if (!raw_value || type != DEPHY_IO_AI ||
        driver_channel >= DEPHY_IO_ZEPHYR_ADC_MAX_CHANNELS ||
        !g_channels[driver_channel].configured) {
        return -1;
    }

    channel = &g_channels[driver_channel];
    if (adc_sequence_init_dt(&channel->spec, &sequence) != 0) {
        return -1;
    }
    sequence.buffer = &channel->sample;
    sequence.buffer_size = sizeof(channel->sample);

    if (adc_read_dt(&channel->spec, &sequence) != 0) {
        return -1;
    }

    *raw_value = channel->sample;
    return 0;
}

static int zephyr_adc_write_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value)
{
    (void)driver_channel;
    (void)type;
    (void)raw_value;
    return -1;
}

static uint32_t zephyr_adc_now_ms(void)
{
    return k_uptime_get_32();
}

const dephy_io_driver_t *dephy_io_zephyr_adc_driver(void)
{
    static const dephy_io_driver_t driver = {
        .init = zephyr_adc_init,
        .read_raw = zephyr_adc_read_raw,
        .write_raw = zephyr_adc_write_raw,
        .now_ms = zephyr_adc_now_ms,
    };

    return &driver;
}

void dephy_io_zephyr_adc_reset(void)
{
    memset(g_channels, 0, sizeof(g_channels));
}

int dephy_io_zephyr_adc_configure(uint16_t driver_channel,
                                  const struct adc_dt_spec *spec)
{
    if (!spec || driver_channel >= DEPHY_IO_ZEPHYR_ADC_MAX_CHANNELS) {
        return -1;
    }

    g_channels[driver_channel].spec = *spec;
    g_channels[driver_channel].configured = 1;
    return 0;
}

int dephy_io_zephyr_adc_raw_to_mv(const struct adc_dt_spec *spec,
                                  int32_t raw,
                                  int32_t *mv)
{
    if (!spec || !mv) {
        return -1;
    }

    *mv = raw;
    return adc_raw_to_millivolts_dt(spec, mv);
}

int dephy_io_zephyr_adc_mv_to_0_10v_x1000(int32_t mv)
{
    return mv;
}

int dephy_io_zephyr_adc_mv_to_4_20ma_x1000(int32_t mv,
                                           int32_t sense_ohms_x1000)
{
    if (sense_ohms_x1000 <= 0) {
        return -1;
    }

    return (int32_t)(((int64_t)mv * 1000000LL) / sense_ohms_x1000);
}

