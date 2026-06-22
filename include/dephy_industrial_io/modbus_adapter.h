#ifndef DEPHY_INDUSTRIAL_IO_MODBUS_ADAPTER_H
#define DEPHY_INDUSTRIAL_IO_MODBUS_ADAPTER_H

#include <stdint.h>

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*dephy_io_modbus_read_cb_t)(uint8_t unit_id,
                                         uint16_t address,
                                         uint16_t *value,
                                         void *user);
typedef int (*dephy_io_modbus_write_cb_t)(uint8_t unit_id,
                                          uint16_t address,
                                          uint16_t value,
                                          void *user);

typedef struct {
    uint8_t unit_id;
    uint16_t address;
    int32_t raw_offset;
} dephy_io_modbus_point_t;

typedef struct {
    dephy_io_modbus_read_cb_t read_register;
    dephy_io_modbus_write_cb_t write_register;
    void *user;
} dephy_io_modbus_transport_t;

const dephy_io_driver_t *dephy_io_modbus_driver(void);
void dephy_io_modbus_reset(void);
int dephy_io_modbus_set_transport(const dephy_io_modbus_transport_t *transport);
int dephy_io_modbus_configure(uint16_t driver_channel,
                              const dephy_io_modbus_point_t *point);

#ifdef __cplusplus
}
#endif

#endif

