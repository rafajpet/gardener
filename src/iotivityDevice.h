//
// Created by rafik on 19.5.2019.
//

#ifndef GARDENER_IOTIVITYCLIENT_H
#define GARDENER_IOTIVITYCLIENT_H

#define MAX_IOTIVITY_ADDRESS (100)
#define MAX_CONFIG_STRING_LENGTH  (200)

typedef void* IotivityDevice_t;

typedef struct {
    char[MAX_IOTIVITY_ADDRESS] cloudAddress;
    char[MAX_CONFIG_STRING_LENGTH] deviceRt;
    char[MAX_CONFIG_STRING_LENGTH] deviceName;
    char[MAX_CONFIG_STRING_LENGTH] manufacturer;
} IotivityConfig_t;


int IotivityDevice_init(IotivityDevice_t *iotivityDevice, IotivityConfig_t *config);

int IotivityDevice_register_resource();

int IotivityDevice_start();

int IotivityDevice_destroy();

#endif //GARDENER_IOTIVITYCLIENT_H
