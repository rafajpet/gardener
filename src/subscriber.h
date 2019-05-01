//
// Created by rafik on 5.4.2019.
//

#ifndef GARDENER_SUBSCRIBER_H
#define GARDENER_SUBSCRIBER_H

#define MAX_ADDRESS_LENGTH 100

typedef void* SensorManager_t;

typedef struct {
    char address[MAX_ADDRESS_LENGTH];
} SensorManagerConfig_t;


typedef void (SensorManager_connectionLost)(char* cause);

typedef struct {

    SensorManager_connectionLost *connectionLost;

//
//    typedef void MQTTClient_connectionLost(void* context, char* cause);
    typedef int MQTTClient_messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message);
//    typedef void MQTTClient_deliveryComplete(void* context, MQTTClient_deliveryToken dt);

}DataReceiver_t;

int SensorClient_init(SensorManager_t *client, SensorManagerConfig_t *config);

int SensorClient_start(SensorManager_t *client);

int SensorClient_stop(SensorManager_t *client);

int SensorClient_destroy(SensorManager_t *client);


#endif //GARDENER_SUBSCRIBER_H
