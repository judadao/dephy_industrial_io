#ifndef DEPHY_INDUSTRIAL_IO_H
#define DEPHY_INDUSTRIAL_IO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEPHY_IO_MAX_CHANNELS
#define DEPHY_IO_MAX_CHANNELS 32
#endif

typedef enum {
    DEPHY_IO_DI = 0,
    DEPHY_IO_DO,
    DEPHY_IO_AI,
    DEPHY_IO_AO,
} dephy_io_type_t;

typedef enum {
    DEPHY_IO_EVENT_CHANGED = 0,
    DEPHY_IO_EVENT_RISING,
    DEPHY_IO_EVENT_FALLING,
    DEPHY_IO_EVENT_FAULT,
} dephy_io_event_type_t;

typedef struct {
    const char *name;
    dephy_io_type_t type;
    uint16_t driver_channel;
    uint32_t debounce_ms;
    int32_t scale_num;
    int32_t scale_den;
    int32_t offset;
} dephy_io_channel_config_t;

typedef struct {
    const char *name;
    dephy_io_type_t type;
    int32_t value;
    uint8_t fault;
    uint32_t changed_at_ms;
} dephy_io_sample_t;

typedef struct {
    dephy_io_event_type_t type;
    dephy_io_sample_t sample;
} dephy_io_event_t;

typedef void (*dephy_io_event_cb_t)(const dephy_io_event_t *event, void *user);

typedef struct {
    int (*init)(const dephy_io_channel_config_t *channels, size_t count);
    int (*read_raw)(uint16_t driver_channel, dephy_io_type_t type, int32_t *raw_value);
    int (*write_raw)(uint16_t driver_channel, dephy_io_type_t type, int32_t raw_value);
    uint32_t (*now_ms)(void);
} dephy_io_driver_t;

int dephy_io_set_driver(const dephy_io_driver_t *driver);
int dephy_io_init(const dephy_io_channel_config_t *channels, size_t count);
void dephy_io_set_event_callback(dephy_io_event_cb_t cb, void *user);
int dephy_io_poll(void);
int dephy_io_read(const char *name, dephy_io_sample_t *sample);
int dephy_io_write(const char *name, int32_t value);
size_t dephy_io_channel_count(void);
const dephy_io_channel_config_t *dephy_io_channel_config(size_t index);

#ifdef __cplusplus
}
#endif

#endif

