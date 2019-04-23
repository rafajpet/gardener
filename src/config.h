//
// Created by rap on 4/5/19.
//

#ifndef GARDENER_CONFIG_H
#define GARDENER_CONFIG_H
#include <stdbool.h>

struct Config {
    char *data_topic;
    char *config_topic;
    char *root_ca_cert_file;
};

typedef struct Config Config;


Config *load_config(const char *file);

void free_config(void *config);

#endif //GARDENER_CONFIG_H
