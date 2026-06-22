#include <stdio.h>

#include <dephy_industrial_io/industrial_io.h>
#include <dephy_industrial_io/posix_sim.h>

static const char *event_name(dephy_io_event_type_t type)
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

static const char *type_name(dephy_io_type_t type)
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

static void on_io_event(const dephy_io_event_t *event, void *user)
{
    (void)user;

    printf("event=%s channel=%s type=%s value=%d fault=%u t=%u\n",
           event_name(event->type),
           event->sample.name,
           type_name(event->sample.type),
           event->sample.value,
           event->sample.fault,
           event->sample.changed_at_ms);
}

static void print_sample(const char *label, const char *name)
{
    dephy_io_sample_t sample;

    if (dephy_io_read(name, &sample) == 0) {
        printf("%s channel=%s value=%d fault=%u t=%u\n",
               label,
               sample.name,
               sample.value,
               sample.fault,
               sample.changed_at_ms);
    }
}

int main(void)
{
    static const dephy_io_channel_config_t channels[] = {
        {
            .name = "door",
            .type = DEPHY_IO_DI,
            .driver_channel = 0,
            .debounce_ms = 20,
            .scale_num = 1,
            .scale_den = 1,
        },
        {
            .name = "relay",
            .type = DEPHY_IO_DO,
            .driver_channel = 1,
            .scale_num = 1,
            .scale_den = 1,
        },
        {
            .name = "pressure_ma_x100",
            .type = DEPHY_IO_AI,
            .driver_channel = 2,
            .scale_num = 100,
            .scale_den = 1,
        },
    };
    int32_t relay_raw = 0;

    dephy_io_posix_sim_reset();
    if (dephy_io_set_driver(dephy_io_posix_sim_driver()) != 0) {
        return 1;
    }
    if (dephy_io_init(channels, sizeof(channels) / sizeof(channels[0])) != 0) {
        return 1;
    }

    dephy_io_set_event_callback(on_io_event, 0);

    printf("linux IO simulator start\n");
    print_sample("initial", "door");

    printf("simulate door raw=1 before debounce window\n");
    dephy_io_posix_sim_set_raw(0, 1);
    dephy_io_posix_sim_advance_ms(10);
    dephy_io_poll();
    print_sample("after_10ms", "door");

    printf("simulate door debounce elapsed\n");
    dephy_io_posix_sim_advance_ms(10);
    dephy_io_poll();
    print_sample("after_20ms", "door");

    printf("simulate pressure raw=12\n");
    dephy_io_posix_sim_set_raw(2, 12);
    dephy_io_posix_sim_advance_ms(1);
    dephy_io_poll();
    print_sample("pressure", "pressure_ma_x100");

    printf("write relay=1\n");
    dephy_io_write("relay", 1);
    dephy_io_posix_sim_get_raw(1, &relay_raw);
    printf("relay_raw=%d\n", relay_raw);

    printf("simulate door raw=0\n");
    dephy_io_posix_sim_set_raw(0, 0);
    dephy_io_posix_sim_advance_ms(20);
    dephy_io_poll();
    print_sample("final", "door");

    return 0;
}

