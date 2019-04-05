//
// Created by rap on 4/5/19.
//

#ifndef GARDENER_CONFIG_H
#define GARDENER_CONFIG_H
#include <stdbool.h>

struct Config {
    bool reload_enable;
    char *data_topic;
    char *config_topic;
    char *root_ca_cert_file;
};

bool load_config(const struct Config *config, const char *file);

bool free_config(const struct Config *config);

#endif //GARDENER_CONFIG_H
