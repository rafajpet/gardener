//
// Created by rafik on 8.5.2019.
//
//system
#include <string.h>

//local
#include "config.h"
#include "log.h"

static void print_help(){

}

int Config_default(Config_t *config) {
    log_debug("Default config for application");
    strcpy(config->mqtt_address, DEFAULT_MQTT_ADDRESS);
    config->mqtt_port = DEFAULT_MQTT_PORT;
    config->tls_enable = false;
    strcpy(config->mqtt_cert_file, DEFAULT_MQTT_CERT_FILE);
    strcpy(config->mqtt_cert_key_file, DEFAULT_MQTT_CERT_KEY_FILE);
    strcpy(config->iotivity_endpoint, DEFAULT_IOTIVITY_ENDPOINT);
    strcpy(config->iotivity_storage_path, DEFAULT_IOTIVITY_STORAGE_PATH);

    return CONFIG_SUCCESS;
}

void Config_print(Config_t *config) {
    log_debug("\n Configuration: \n MQTT_ADDRESS: %s \n MQTT_PORT: %d \n TLS_ENABLE: %d \n MQTT_CERT_FILE_PATH: %s \n "
              "MQTT_CERT_KEY_FILE_PATH: %s \n IOTIVITY_ENDPOINT: %s \n IOTIVITY_PATH: %s \n"
            ,config->mqtt_address, config->mqtt_port, config->tls_enable, config->mqtt_cert_file
            ,config->mqtt_cert_key_file, config->iotivity_endpoint, config->iotivity_storage_path);
}

int Config_set_parameter(Config_t *config, int argc, char *argv[]) {

    return CONFIG_SUCCESS;
}
