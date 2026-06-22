#include <dephy_industrial_io/mqtt_bridge.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dephy_industrial_io/payload.h>

#ifndef DEPHY_IO_MQTT_TOPIC_MAX
#define DEPHY_IO_MQTT_TOPIC_MAX 160
#endif

#ifndef DEPHY_IO_MQTT_PAYLOAD_MAX
#define DEPHY_IO_MQTT_PAYLOAD_MAX 160
#endif

static dephy_io_mqtt_bridge_config_t g_bridge;

static int publish_payload(const char *channel, const char *suffix, const char *payload)
{
    char topic[DEPHY_IO_MQTT_TOPIC_MAX];

    if (!g_bridge.publish ||
        dephy_io_format_topic(topic,
                              sizeof(topic),
                              g_bridge.site,
                              g_bridge.node,
                              channel,
                              suffix) < 0) {
        return -1;
    }

    return g_bridge.publish(topic, payload, g_bridge.user);
}

int dephy_io_mqtt_bridge_init(const dephy_io_mqtt_bridge_config_t *config)
{
    if (!config || !config->site || !config->node || !config->publish) {
        return -1;
    }

    g_bridge = *config;
    return 0;
}

void dephy_io_mqtt_bridge_handle_event(const dephy_io_event_t *event, void *user)
{
    char payload[DEPHY_IO_MQTT_PAYLOAD_MAX];
    const char *suffix;

    (void)user;

    if (!event || !event->sample.name ||
        dephy_io_format_event_json(payload, sizeof(payload), event) < 0) {
        return;
    }

    suffix = event->sample.fault ? "fault" : "event";
    (void)publish_payload(event->sample.name, suffix, payload);

    if (g_bridge.publish_state_on_event) {
        (void)dephy_io_mqtt_bridge_publish_state(event->sample.name);
    }
}

int dephy_io_mqtt_bridge_publish_state(const char *channel_name)
{
    dephy_io_sample_t sample;
    char payload[DEPHY_IO_MQTT_PAYLOAD_MAX];

    if (!channel_name ||
        dephy_io_read(channel_name, &sample) != 0 ||
        dephy_io_format_sample_json(payload, sizeof(payload), &sample) < 0) {
        return -1;
    }

    return publish_payload(channel_name, "state", payload);
}

int dephy_io_mqtt_bridge_publish_all_states(void)
{
    size_t i;
    int rc = 0;

    for (i = 0; i < dephy_io_channel_count(); ++i) {
        const dephy_io_channel_config_t *cfg = dephy_io_channel_config(i);
        if (!cfg || dephy_io_mqtt_bridge_publish_state(cfg->name) != 0) {
            rc = -1;
        }
    }

    return rc;
}

int dephy_io_mqtt_bridge_handle_command(const char *topic, const char *payload)
{
    char prefix[DEPHY_IO_MQTT_TOPIC_MAX];
    const char *channel;
    const char *suffix;
    char *end = 0;
    long value;
    int n;

    if (!topic || !payload || !g_bridge.site || !g_bridge.node) {
        return -1;
    }

    n = snprintf(prefix,
                 sizeof(prefix),
                 "site/%s/node/%s/io/",
                 g_bridge.site,
                 g_bridge.node);
    if (n < 0 || (size_t)n >= sizeof(prefix) ||
        strncmp(topic, prefix, (size_t)n) != 0) {
        return -1;
    }

    channel = topic + n;
    suffix = strstr(channel, "/set");
    if (!suffix || suffix[4] != '\0' || suffix == channel) {
        return -1;
    }

    value = strtol(payload, &end, 10);
    if (end == payload) {
        return -1;
    }

    {
        char name[64];
        size_t len = (size_t)(suffix - channel);
        if (len >= sizeof(name)) {
            return -1;
        }
        memcpy(name, channel, len);
        name[len] = '\0';
        return dephy_io_write(name, (int32_t)value);
    }
}

