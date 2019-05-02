//
// Created by rafik on 5.4.2019.
//

#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subscriber.h"
#include "log.h"

#include "MQTTClient.h"

#define CLIENTID    "sensor-manager-client"
#define QOS         1
#define TIMEOUT     2000

typedef struct {

    SensorManagerConfig_t *config;
    MQTTClient *mqttClient;
    MQTTClient_connectOptions connectOptions;

} Manager_t;


void message_delivered(void *context, MQTTClient_deliveryToken dt)
{
    log_info("message with token  %d delivery confirmed\n", dt);
}

void connection_lost(void *context, char *cause)
{
    log_error("connection lost,cause: %s", cause);
}

int SensorManager_init(SensorManager_t *client, SensorManagerConfig_t *config) {

    log_debug("start init sensor manager");
    Manager_t *c = malloc(sizeof(Manager_t));
    //init MQTT client
    log_debug("init mqtt client start");
    MQTTClient *mqttClient = malloc(sizeof(MQTTClient));
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(mqttClient, config->address, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    log_debug("Create mqtt client for connecting to: %s", config->address);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    log_debug("finish init sensor manager");
    c->config = config;
    c->mqttClient = mqttClient;
    c->connectOptions = conn_opts;
    *client = c;
    return 0;
}

int SensorManager_set_receive(SensorManager_t *client, receive_message_t *receiver) {
    log_debug("set receive callback");

    Manager_t* sensorClient = *client;
    MQTTClient_setCallbacks(*(sensorClient->mqttClient), NULL, connection_lost , receiver, message_delivered);
    return 0;
}

int SensorManager_start(SensorManager_t* client) {

    log_debug("start sensor manager");
    Manager_t* sensorClient = *client;

    int rc;
    if ((rc = MQTTClient_connect(*(sensorClient->mqttClient), &sensorClient->connectOptions)) != MQTTCLIENT_SUCCESS)
    {
        log_error("Failed to connect, return code %d\n", rc);
        return rc;
    }
    log_debug("subscribe to topic: %s", sensorClient->config->topic);
    MQTTClient_subscribe(*(sensorClient->mqttClient), sensorClient->config->topic, QOS);


    return 0;
}

int SensorManager_stop(SensorManager_t *client) {

    log_debug("stop sensor manager");
    Manager_t* sensorClient = *client;
    MQTTClient_unsubscribe(*(sensorClient->mqttClient), sensorClient->config->topic);
    log_debug("unsubscribe from topic");
    MQTTClient_disconnect(*(sensorClient->mqttClient),TIMEOUT);
    log_debug("disconnect from mqtt broker");
    return 0;
}

int SensorManager_destroy(SensorManager_t *client) {

    log_debug("free sensor manager");
    Manager_t* sensorClient = *client;
    MQTTClient_destroy(sensorClient->mqttClient);
    free(sensorClient->mqttClient);
    free(sensorClient);
    *client = NULL;
    return 0;
}

//pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t  condition_var   = PTHREAD_COND_INITIALIZER;
//
//
//
//
//
//void *subscribe_function(void *param)
//{
//    MQTTClient client;
//    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//    int rc;
//    int ch;
//
//    MQTTClient_create(&client, ADDRESS, CLIENTID,
//                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
//    conn_opts.keepAliveInterval = 20;
//    conn_opts.cleansession = 1;
//
//    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
//
//    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
//    {
//        printf("Failed to connect, return code %d\n", rc);
//        return NULL;
//    }
//    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
//           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
//    MQTTClient_subscribe(client, TOPIC, QOS);
//
//    pthread_mutex_lock( &count_mutex );
//    pthread_cond_wait( &condition_var, &count_mutex );
//    pthread_mutex_unlock( &count_mutex );
//
//    log_debug("close mqtt subscribe to topic");
//    MQTTClient_unsubscribe(client, TOPIC);
//    MQTTClient_disconnect(client, 10000);
//    MQTTClient_destroy(&client);
//    return NULL;
//}
//
//void stop_subscribe(){
//    log_debug("stop subscribe");
//    pthread_mutex_lock(&count_mutex);
//    pthread_cond_signal(&condition_var);
//    pthread_mutex_unlock(&count_mutex);
//    log_debug("stop subscribe OK");
//}