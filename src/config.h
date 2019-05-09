//
// Created by rafik on 8.5.2019.
//

#ifndef GARDENER_CONFIG_H
#define GARDENER_CONFIG_H

#include <linux/limits.h>
#include <stdbool.h>

#define MAX_ADDRESS_LENGTH 50
#define MAX_TOPIC_LENGTH 50

#define OK 0

// ok
#define CONFIG_SUCCESS 0
// invalid option
#define CONFIG_INVALID_INPUT -1
// error
#define CONFIG_ERROR -2

//default values
#define DEFAULT_MQTT_ADDRESS "localhost"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_CERT_FILE "cert.pem"
#define DEFAULT_MQTT_CERT_KEY_FILE "cert-key.pem"
#define DEFAULT_IOTIVITY_ENDPOINT "coap+tcp://localhost:5684"
#define DEFAULT_IOTIVITY_STORAGE_PATH "./"

typedef struct {
    //address of mqtt server
    char mqtt_address[MAX_ADDRESS_LENGTH];
    //mqtt port
    int  mqtt_port;
    //mqtt topic to connect
    char mqtt_topic[MAX_TOPIC_LENGTH];
    bool tls_enable;
    char mqtt_cert_file[PATH_MAX];
    char mqtt_cert_key_file[PATH_MAX];
    //iotivity hostname:port
    char iotivity_endpoint[MAX_ADDRESS_LENGTH];
    //iotivity working directory
    char iotivity_storage_path[PATH_MAX];

} Config_t;

int Config_default(Config_t *config);

void Config_print(Config_t *config);

int Config_set_parameter(Config_t *config, int argc, char *argv[]);

int Config_get_mqtt_address(Config_t *config, char *address);

#endif //GARDENER_CONFIG_H
