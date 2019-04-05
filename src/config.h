//
// Created by rap on 4/5/19.
//

#ifndef GARDENER_CONFIG_H
#define GARDENER_CONFIG_H

struct Config {
    char *data_topic;
    char *config_topic;
    char *root_ca_cert_file;
};

int load_config(const struct Config *config, const char *file);



#endif //GARDENER_CONFIG_H
