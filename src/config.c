//
// Created by rap on 4/5/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"

#define MAX_TOPIC_LEN  50
#define  MAX_CERT 10

#define CONFIG_TOPIC  "test"
#define DATA_TOPIC    "data"


Config *load_config(const char* file) {

    Config *config = (Config*)malloc(sizeof(Config *));
    if (config == NULL){
        log_error("unable to create ");
        return NULL;
    }
    log_debug("start loading configuration from: %s", file);
    config->data_topic = malloc(MAX_TOPIC_LEN * sizeof(char *));
    strcpy(config->data_topic, DATA_TOPIC);

    config->config_topic = malloc(MAX_TOPIC_LEN * sizeof(char *));
    strcpy(config->config_topic, CONFIG_TOPIC);

    config->root_ca_cert_file = malloc(MAX_CERT * sizeof(char *));
    strcpy(config->root_ca_cert_file, "haha cert");

    log_debug("stop loading configuration from: %s", file);
    return config;
}

void free_config(void *config){
    log_debug("free config file start");
    Config *c = (Config *)config;
    free(c->root_ca_cert_file);
    free(c->config_topic);
    free(c->data_topic);
    free(c);
    log_debug("free config file end");
}

