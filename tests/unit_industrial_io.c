#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <dephy_industrial_io/industrial_io.h>
#include <dephy_industrial_io/posix_sim.h>

static int g_events;
static dephy_io_event_t g_last_event;

static void on_event(const dephy_io_event_t *event, void *user)
{
    (void)user;
    g_events++;
    g_last_event = *event;
}

static void test_di_edges_and_debounce(void)
{
    dephy_io_channel_config_t channels[] = {
        {
            .name = "door",
            .type = DEPHY_IO_DI,
            .driver_channel = 1,
            .debounce_ms = 10,
            .scale_num = 1,
            .scale_den = 1,
        },
    };
    dephy_io_sample_t sample;

    dephy_io_posix_sim_reset();
    assert(dephy_io_set_driver(dephy_io_posix_sim_driver()) == 0);
    assert(dephy_io_init(channels, 1) == 0);
    dephy_io_set_event_callback(on_event, 0);
    g_events = 0;

    assert(dephy_io_posix_sim_set_raw(1, 1) == 0);
    dephy_io_posix_sim_advance_ms(5);
    assert(dephy_io_poll() == 0);
    assert(g_events == 0);

    dephy_io_posix_sim_advance_ms(5);
    assert(dephy_io_poll() == 0);
    assert(g_events == 1);
    assert(g_last_event.type == DEPHY_IO_EVENT_RISING);
    assert(strcmp(g_last_event.sample.name, "door") == 0);

    assert(dephy_io_read("door", &sample) == 0);
    assert(sample.value == 1);

    assert(dephy_io_posix_sim_set_raw(1, 0) == 0);
    dephy_io_posix_sim_advance_ms(10);
    assert(dephy_io_poll() == 0);
    assert(g_events == 2);
    assert(g_last_event.type == DEPHY_IO_EVENT_FALLING);
}

static void test_output_write(void)
{
    dephy_io_channel_config_t channels[] = {
        {
            .name = "relay",
            .type = DEPHY_IO_DO,
            .driver_channel = 2,
            .scale_num = 1,
            .scale_den = 1,
        },
    };
    int32_t raw = 0;

    dephy_io_posix_sim_reset();
    assert(dephy_io_set_driver(dephy_io_posix_sim_driver()) == 0);
    assert(dephy_io_init(channels, 1) == 0);
    g_events = 0;

    assert(dephy_io_write("relay", 1) == 0);
    assert(dephy_io_posix_sim_get_raw(2, &raw) == 0);
    assert(raw == 1);
}

static void test_analog_output_write_scaling(void)
{
    dephy_io_channel_config_t channels[] = {
        {
            .name = "valve",
            .type = DEPHY_IO_AO,
            .driver_channel = 4,
            .scale_num = 10,
            .scale_den = 1,
        },
    };
    int32_t raw = 0;
    dephy_io_sample_t sample;

    dephy_io_posix_sim_reset();
    assert(dephy_io_set_driver(dephy_io_posix_sim_driver()) == 0);
    assert(dephy_io_init(channels, 1) == 0);

    assert(dephy_io_write("valve", 70) == 0);
    assert(dephy_io_posix_sim_get_raw(4, &raw) == 0);
    assert(raw == 7);
    assert(dephy_io_read("valve", &sample) == 0);
    assert(sample.value == 70);
    assert(sample.fault == 0);
}

static void test_sim_fault_and_stuck(void)
{
    dephy_io_channel_config_t channels[] = {
        {
            .name = "pressure",
            .type = DEPHY_IO_AI,
            .driver_channel = 3,
            .scale_num = 10,
            .scale_den = 1,
        },
    };
    dephy_io_sample_t sample;

    dephy_io_posix_sim_reset();
    assert(dephy_io_set_driver(dephy_io_posix_sim_driver()) == 0);
    assert(dephy_io_init(channels, 1) == 0);
    dephy_io_set_event_callback(on_event, 0);
    g_events = 0;

    assert(dephy_io_posix_sim_set_fault(3, 1) == 0);
    assert(dephy_io_poll() == 0);
    assert(g_events == 1);
    assert(g_last_event.type == DEPHY_IO_EVENT_FAULT);
    assert(dephy_io_read("pressure", &sample) == 0);
    assert(sample.fault == 1);

    assert(dephy_io_posix_sim_set_fault(3, 0) == 0);
    assert(dephy_io_posix_sim_set_stuck(3, 1, 7) == 0);
    assert(dephy_io_posix_sim_set_raw(3, 12) == 0);
    dephy_io_posix_sim_advance_ms(1);
    assert(dephy_io_poll() == 0);
    assert(dephy_io_read("pressure", &sample) == 0);
    assert(sample.value == 70);
    assert(sample.fault == 0);
}

int main(void)
{
    test_di_edges_and_debounce();
    test_output_write();
    test_analog_output_write_scaling();
    test_sim_fault_and_stuck();
    printf("industrial_io unit tests passed\n");
    return 0;
}
