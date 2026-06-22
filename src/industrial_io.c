#include <dephy_industrial_io/industrial_io.h>

#include <string.h>

typedef struct {
    dephy_io_channel_config_t cfg;
    dephy_io_sample_t sample;
    int initialized;
} io_slot_t;

static const dephy_io_driver_t *g_driver;
static io_slot_t g_slots[DEPHY_IO_MAX_CHANNELS];
static size_t g_count;
static dephy_io_event_cb_t g_event_cb;
static void *g_event_user;

static uint32_t io_now_ms(void)
{
    if (g_driver && g_driver->now_ms) {
        return g_driver->now_ms();
    }
    return 0;
}

static int is_input_type(dephy_io_type_t type)
{
    return type == DEPHY_IO_DI || type == DEPHY_IO_AI;
}

static int is_output_type(dephy_io_type_t type)
{
    return type == DEPHY_IO_DO || type == DEPHY_IO_AO;
}

static int scale_from_raw(const dephy_io_channel_config_t *cfg, int32_t raw, int32_t *value)
{
    int32_t den = cfg->scale_den == 0 ? 1 : cfg->scale_den;
    *value = (int32_t)(((int64_t)raw * cfg->scale_num) / den) + cfg->offset;
    return 0;
}

static int raw_from_scale(const dephy_io_channel_config_t *cfg, int32_t value, int32_t *raw)
{
    int32_t num = cfg->scale_num == 0 ? 1 : cfg->scale_num;
    *raw = (int32_t)(((int64_t)(value - cfg->offset) * cfg->scale_den) / num);
    return 0;
}

static int find_channel(const char *name)
{
    size_t i;

    if (!name) {
        return -1;
    }

    for (i = 0; i < g_count; ++i) {
        if (g_slots[i].cfg.name && strcmp(g_slots[i].cfg.name, name) == 0) {
            return (int)i;
        }
    }

    return -1;
}

static void emit_event(dephy_io_event_type_t type, const dephy_io_sample_t *sample)
{
    dephy_io_event_t event;

    if (!g_event_cb) {
        return;
    }

    event.type = type;
    event.sample = *sample;
    g_event_cb(&event, g_event_user);
}

int dephy_io_set_driver(const dephy_io_driver_t *driver)
{
    if (!driver || !driver->read_raw || !driver->write_raw) {
        return -1;
    }

    g_driver = driver;
    return 0;
}

int dephy_io_init(const dephy_io_channel_config_t *channels, size_t count)
{
    size_t i;

    if (!g_driver || !channels || count > DEPHY_IO_MAX_CHANNELS) {
        return -1;
    }

    memset(g_slots, 0, sizeof(g_slots));
    g_count = count;

    for (i = 0; i < count; ++i) {
        if (!channels[i].name || channels[i].scale_den == 0) {
            g_count = 0;
            return -1;
        }

        g_slots[i].cfg = channels[i];
        g_slots[i].sample.name = channels[i].name;
        g_slots[i].sample.type = channels[i].type;
        g_slots[i].sample.changed_at_ms = io_now_ms();
        g_slots[i].initialized = 1;
    }

    if (g_driver->init && g_driver->init(channels, count) != 0) {
        g_count = 0;
        return -1;
    }

    return dephy_io_poll();
}

void dephy_io_set_event_callback(dephy_io_event_cb_t cb, void *user)
{
    g_event_cb = cb;
    g_event_user = user;
}

int dephy_io_poll(void)
{
    size_t i;

    if (!g_driver) {
        return -1;
    }

    for (i = 0; i < g_count; ++i) {
        io_slot_t *slot = &g_slots[i];
        int32_t raw = 0;
        int32_t value = 0;
        uint32_t now = io_now_ms();
        dephy_io_event_type_t event_type = DEPHY_IO_EVENT_CHANGED;

        if (!is_input_type(slot->cfg.type)) {
            continue;
        }

        if (g_driver->read_raw(slot->cfg.driver_channel, slot->cfg.type, &raw) != 0) {
            slot->sample.fault = 1;
            emit_event(DEPHY_IO_EVENT_FAULT, &slot->sample);
            continue;
        }

        scale_from_raw(&slot->cfg, raw, &value);
        if (value == slot->sample.value && slot->sample.fault == 0) {
            continue;
        }

        if (slot->cfg.debounce_ms > 0 &&
            now - slot->sample.changed_at_ms < slot->cfg.debounce_ms) {
            continue;
        }

        if (slot->cfg.type == DEPHY_IO_DI) {
            if (slot->sample.value == 0 && value != 0) {
                event_type = DEPHY_IO_EVENT_RISING;
            } else if (slot->sample.value != 0 && value == 0) {
                event_type = DEPHY_IO_EVENT_FALLING;
            }
        }

        slot->sample.value = value;
        slot->sample.fault = 0;
        slot->sample.changed_at_ms = now;
        emit_event(event_type, &slot->sample);
    }

    return 0;
}

int dephy_io_read(const char *name, dephy_io_sample_t *sample)
{
    int idx = find_channel(name);

    if (idx < 0 || !sample) {
        return -1;
    }

    *sample = g_slots[idx].sample;
    return 0;
}

int dephy_io_write(const char *name, int32_t value)
{
    int idx = find_channel(name);
    int32_t raw = 0;
    io_slot_t *slot;

    if (idx < 0 || !g_driver) {
        return -1;
    }

    slot = &g_slots[idx];
    if (!is_output_type(slot->cfg.type)) {
        return -1;
    }

    raw_from_scale(&slot->cfg, value, &raw);
    if (g_driver->write_raw(slot->cfg.driver_channel, slot->cfg.type, raw) != 0) {
        slot->sample.fault = 1;
        emit_event(DEPHY_IO_EVENT_FAULT, &slot->sample);
        return -1;
    }

    slot->sample.value = value;
    slot->sample.fault = 0;
    slot->sample.changed_at_ms = io_now_ms();
    emit_event(DEPHY_IO_EVENT_CHANGED, &slot->sample);
    return 0;
}

size_t dephy_io_channel_count(void)
{
    return g_count;
}

const dephy_io_channel_config_t *dephy_io_channel_config(size_t index)
{
    if (index >= g_count) {
        return 0;
    }

    return &g_slots[index].cfg;
}
