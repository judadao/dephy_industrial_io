#ifndef DEPHY_INDUSTRIAL_IO_PAYLOAD_H
#define DEPHY_INDUSTRIAL_IO_PAYLOAD_H

#include <stddef.h>

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *dephy_io_type_name(dephy_io_type_t type);
const char *dephy_io_event_name(dephy_io_event_type_t type);

int dephy_io_format_topic(char *out,
                          size_t out_size,
                          const char *site,
                          const char *node,
                          const char *channel,
                          const char *suffix);

int dephy_io_format_sample_json(char *out,
                                size_t out_size,
                                const dephy_io_sample_t *sample);

int dephy_io_format_event_json(char *out,
                               size_t out_size,
                               const dephy_io_event_t *event);

#ifdef __cplusplus
}
#endif

#endif

