#ifndef DEPHY_INDUSTRIAL_IO_MQTT_BRIDGE_H
#define DEPHY_INDUSTRIAL_IO_MQTT_BRIDGE_H

#include <stddef.h>

#include <dephy_industrial_io/industrial_io.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*dephy_io_mqtt_publish_cb_t)(const char *topic,
                                          const char *payload,
                                          void *user);

typedef struct {
    const char *site;
    const char *node;
    dephy_io_mqtt_publish_cb_t publish;
    void *user;
    uint8_t publish_state_on_event;
} dephy_io_mqtt_bridge_config_t;

int dephy_io_mqtt_bridge_init(const dephy_io_mqtt_bridge_config_t *config);
void dephy_io_mqtt_bridge_handle_event(const dephy_io_event_t *event, void *user);
int dephy_io_mqtt_bridge_publish_state(const char *channel_name);
int dephy_io_mqtt_bridge_publish_all_states(void);
int dephy_io_mqtt_bridge_handle_command(const char *topic, const char *payload);

#ifdef __cplusplus
}
#endif

#endif

