#include <dephy_industrial_io/modbus_adapter.h>

#include <string.h>

#ifndef DEPHY_IO_MODBUS_MAX_POINTS
#define DEPHY_IO_MODBUS_MAX_POINTS 128
#endif

typedef struct {
    dephy_io_modbus_point_t point;
    uint8_t configured;
} modbus_slot_t;

static modbus_slot_t g_slots[DEPHY_IO_MODBUS_MAX_POINTS];
static dephy_io_modbus_transport_t g_transport;

static int modbus_init(const dephy_io_channel_config_t *channels, size_t count)
{
    size_t i;

    if (!g_transport.read_register || !g_transport.write_register) {
        return -1;
    }

    for (i = 0; i < count; ++i) {
        if (channels[i].driver_channel >= DEPHY_IO_MODBUS_MAX_POINTS ||
            !g_slots[channels[i].driver_channel].configured) {
            return -1;
        }
    }

    return 0;
}

static int modbus_read_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t *raw_value)
{
    uint16_t reg = 0;
    modbus_slot_t *slot;

    (void)type;

    if (!raw_value || driver_channel >= DEPHY_IO_MODBUS_MAX_POINTS ||
        !g_slots[driver_channel].configured || !g_transport.read_register) {
        return -1;
    }

    slot = &g_slots[driver_channel];
    if (g_transport.read_register(slot->point.unit_id,
                                  slot->point.address,
                                  &reg,
                                  g_transport.user) != 0) {
        return -1;
    }

    *raw_value = (int32_t)reg + slot->point.raw_offset;
    return 0;
}

static int modbus_write_raw(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value)
{
    modbus_slot_t *slot;
    int32_t adjusted;

    (void)type;

    if (driver_channel >= DEPHY_IO_MODBUS_MAX_POINTS ||
        !g_slots[driver_channel].configured || !g_transport.write_register) {
        return -1;
    }

    slot = &g_slots[driver_channel];
    adjusted = raw_value - slot->point.raw_offset;
    if (adjusted < 0 || adjusted > 0xffff) {
        return -1;
    }

    return g_transport.write_register(slot->point.unit_id,
                                      slot->point.address,
                                      (uint16_t)adjusted,
                                      g_transport.user);
}

const dephy_io_driver_t *dephy_io_modbus_driver(void)
{
    static const dephy_io_driver_t driver = {
        .init = modbus_init,
        .read_raw = modbus_read_raw,
        .write_raw = modbus_write_raw,
        .now_ms = 0,
    };

    return &driver;
}

void dephy_io_modbus_reset(void)
{
    memset(g_slots, 0, sizeof(g_slots));
    memset(&g_transport, 0, sizeof(g_transport));
}

int dephy_io_modbus_set_transport(const dephy_io_modbus_transport_t *transport)
{
    if (!transport || !transport->read_register || !transport->write_register) {
        return -1;
    }

    g_transport = *transport;
    return 0;
}

int dephy_io_modbus_configure(uint16_t driver_channel,
                              const dephy_io_modbus_point_t *point)
{
    if (!point || driver_channel >= DEPHY_IO_MODBUS_MAX_POINTS) {
        return -1;
    }

    g_slots[driver_channel].point = *point;
    g_slots[driver_channel].configured = 1;
    return 0;
}

