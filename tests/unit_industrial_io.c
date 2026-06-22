#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <dephy_industrial_io/industrial_io.h>
#include <dephy_industrial_io/modbus_adapter.h>
#include <dephy_industrial_io/mqtt_bridge.h>
#include <dephy_industrial_io/payload.h>
#include <dephy_industrial_io/posix_sim.h>

static int g_events;
static dephy_io_event_t g_last_event;
static int g_pub_count;
static char g_last_topic[160];
static char g_last_payload[160];
static uint16_t g_modbus_regs[16];

static void on_event(const dephy_io_event_t *event, void *user)
{
    (void)user;
    g_events++;
    g_last_event = *event;
}

static int capture_publish(const char *topic, const char *payload, void *user)
{
    (void)user;
    g_pub_count++;
    snprintf(g_last_topic, sizeof(g_last_topic), "%s", topic);
    snprintf(g_last_payload, sizeof(g_last_payload), "%s", payload);
    return 0;
}

static int modbus_read(uint8_t unit_id, uint16_t address, uint16_t *value, void *user)
{
    (void)unit_id;
    (void)user;
    if (!value || address >= 16) {
        return -1;
    }
    *value = g_modbus_regs[address];
    return 0;
}

static int modbus_write(uint8_t unit_id, uint16_t address, uint16_t value, void *user)
{
    (void)unit_id;
    (void)user;
    if (address >= 16) {
        return -1;
    }
    g_modbus_regs[address] = value;
    return 0;
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

static void test_payload_and_mqtt_bridge(void)
{
    dephy_io_channel_config_t channels[] = {
        {
            .name = "relay",
            .type = DEPHY_IO_DO,
            .driver_channel = 5,
            .scale_num = 1,
            .scale_den = 1,
        },
    };
    dephy_io_mqtt_bridge_config_t cfg = {
        .site = "factory-a",
        .node = "node-1",
        .publish = capture_publish,
        .publish_state_on_event = 1,
    };
    char topic[128];
    char payload[128];

    dephy_io_posix_sim_reset();
    assert(dephy_io_set_driver(dephy_io_posix_sim_driver()) == 0);
    assert(dephy_io_init(channels, 1) == 0);
    assert(dephy_io_format_topic(topic, sizeof(topic),
                                 "factory-a", "node-1", "relay", "state") > 0);
    assert(strcmp(topic, "site/factory-a/node/node-1/io/relay/state") == 0);

    g_pub_count = 0;
    assert(dephy_io_mqtt_bridge_init(&cfg) == 0);
    dephy_io_set_event_callback(dephy_io_mqtt_bridge_handle_event, 0);
    assert(dephy_io_mqtt_bridge_handle_command(
               "site/factory-a/node/node-1/io/relay/set", "1") == 0);
    assert(g_pub_count == 2);
    assert(strcmp(g_last_topic, "site/factory-a/node/node-1/io/relay/state") == 0);
    assert(strstr(g_last_payload, "\"value\":1") != 0);

    dephy_io_sample_t sample;
    assert(dephy_io_read("relay", &sample) == 0);
    assert(dephy_io_format_sample_json(payload, sizeof(payload), &sample) > 0);
    assert(strcmp(payload, "{\"type\":\"do\",\"value\":1,\"fault\":0,\"t_ms\":0}") == 0);
}

static void test_modbus_adapter(void)
{
    dephy_io_modbus_transport_t transport = {
        .read_register = modbus_read,
        .write_register = modbus_write,
    };
    dephy_io_modbus_point_t point = {
        .unit_id = 1,
        .address = 3,
        .raw_offset = 10,
    };
    dephy_io_channel_config_t channels[] = {
        {
            .name = "holding",
            .type = DEPHY_IO_AI,
            .driver_channel = 0,
            .scale_num = 2,
            .scale_den = 1,
        },
        {
            .name = "command",
            .type = DEPHY_IO_AO,
            .driver_channel = 0,
            .scale_num = 2,
            .scale_den = 1,
        },
    };
    dephy_io_sample_t sample;

    memset(g_modbus_regs, 0, sizeof(g_modbus_regs));
    g_modbus_regs[3] = 5;
    dephy_io_modbus_reset();
    assert(dephy_io_modbus_set_transport(&transport) == 0);
    assert(dephy_io_modbus_configure(0, &point) == 0);
    assert(dephy_io_set_driver(dephy_io_modbus_driver()) == 0);
    assert(dephy_io_init(channels, 1) == 0);
    assert(dephy_io_read("holding", &sample) == 0);
    assert(sample.value == 30);

    assert(dephy_io_init(&channels[1], 1) == 0);
    assert(dephy_io_write("command", 40) == 0);
    assert(g_modbus_regs[3] == 10);
}

int main(void)
{
    test_di_edges_and_debounce();
    test_output_write();
    test_analog_output_write_scaling();
    test_sim_fault_and_stuck();
    test_payload_and_mqtt_bridge();
    test_modbus_adapter();
    printf("industrial_io unit tests passed\n");
    return 0;
}
