//
// Created by rafik on 5.4.2019.
//

#ifndef GARDENER_SUBSCRIBER_H
#define GARDENER_SUBSCRIBER_H

#include <MQTTClient.h>

#define MAX_ADDRESS_LENGTH 100
#define MAX_TOPIC_LENGTH 100

typedef void* SensorManager_t;

typedef struct {
    char address[MAX_ADDRESS_LENGTH];
    char topic[MAX_TOPIC_LENGTH];
} SensorManagerConfig_t;


typedef int receive_message_t(void *context, char *topicName, int topicLen, MQTTClient_message *message);


int SensorManager_init(SensorManager_t *client, SensorManagerConfig_t *config);

int SensorManager_set_receive(SensorManager_t *client, receive_message_t *receiver);

int SensorManager_start(SensorManager_t *client);

int SensorManager_stop(SensorManager_t *client);

int SensorManager_destroy(SensorManager_t *client);


#endif //GARDENER_SUBSCRIBER_H
