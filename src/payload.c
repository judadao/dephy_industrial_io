#include <dephy_industrial_io/payload.h>

#include <stdio.h>

const char *dephy_io_type_name(dephy_io_type_t type)
{
    switch (type) {
    case DEPHY_IO_DI:
        return "di";
    case DEPHY_IO_DO:
        return "do";
    case DEPHY_IO_AI:
        return "ai";
    case DEPHY_IO_AO:
        return "ao";
    default:
        return "unknown";
    }
}

const char *dephy_io_event_name(dephy_io_event_type_t type)
{
    switch (type) {
    case DEPHY_IO_EVENT_CHANGED:
        return "changed";
    case DEPHY_IO_EVENT_RISING:
        return "rising";
    case DEPHY_IO_EVENT_FALLING:
        return "falling";
    case DEPHY_IO_EVENT_FAULT:
        return "fault";
    default:
        return "unknown";
    }
}

int dephy_io_format_topic(char *out,
                          size_t out_size,
                          const char *site,
                          const char *node,
                          const char *channel,
                          const char *suffix)
{
    int n;

    if (!out || out_size == 0 || !site || !node || !channel || !suffix) {
        return -1;
    }

    n = snprintf(out,
                 out_size,
                 "site/%s/node/%s/io/%s/%s",
                 site,
                 node,
                 channel,
                 suffix);
    if (n < 0 || (size_t)n >= out_size) {
        return -1;
    }

    return n;
}

int dephy_io_format_sample_json(char *out,
                                size_t out_size,
                                const dephy_io_sample_t *sample)
{
    int n;

    if (!out || out_size == 0 || !sample || !sample->name) {
        return -1;
    }

    n = snprintf(out,
                 out_size,
                 "{\"type\":\"%s\",\"value\":%d,\"fault\":%u,\"t_ms\":%u}",
                 dephy_io_type_name(sample->type),
                 sample->value,
                 sample->fault,
                 sample->changed_at_ms);
    if (n < 0 || (size_t)n >= out_size) {
        return -1;
    }

    return n;
}

int dephy_io_format_event_json(char *out,
                               size_t out_size,
                               const dephy_io_event_t *event)
{
    int n;

    if (!out || out_size == 0 || !event || !event->sample.name) {
        return -1;
    }

    n = snprintf(out,
                 out_size,
                 "{\"event\":\"%s\",\"type\":\"%s\",\"value\":%d,"
                 "\"fault\":%u,\"t_ms\":%u}",
                 dephy_io_event_name(event->type),
                 dephy_io_type_name(event->sample.type),
                 event->sample.value,
                 event->sample.fault,
                 event->sample.changed_at_ms);
    if (n < 0 || (size_t)n >= out_size) {
        return -1;
    }

    return n;
}

