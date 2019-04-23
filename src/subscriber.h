//
// Created by rafik on 5.4.2019.
//

#ifndef GARDENER_SUBSCRIBER_H
#define GARDENER_SUBSCRIBER_H


typedef void* SensorClient;

typedef struct {

} SensorClientConfig;

int SensorClient_init(SensorClient *client, SensorClientConfig *config);

int SensorClient_start(SensorClient *client);

int SensorClient_stop(SensorClient *client);

int SensorClient_free(SensorClient *client);


#endif //GARDENER_SUBSCRIBER_H
