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

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientSub"
#define TOPIC       "test"
#define TOPIC2      "topic2"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L


void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");

    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}


typedef struct {

    MQTTClient *mqttClient;
    MQTTClient_connectOptions connectOptions;

} Manager_t;

int SensorClient_init(SensorManager_t *client, SensorManagerConfig_t *config) {

    log_debug("start init sensor manager");
    Manager_t *c = malloc(sizeof(Manager_t));
    //init MQTT client
    log_debug("init mqtt client start");
    MQTTClient *mqttClient = malloc(sizeof(MQTTClient));
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(mqttClient, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(*mqttClient, NULL, connlost, msgarrvd, delivered);
    log_debug("finish init sensor manager");
    c->mqttClient = mqttClient;
    c->connectOptions = conn_opts;
    *client = c;
    return 0;
}

int SensorClient_start(SensorManager_t* client) {

    log_debug("start sensor manager");
    Manager_t* sensorClient = *client;

    int rc;
    if ((rc = MQTTClient_connect(*(sensorClient->mqttClient), &sensorClient->connectOptions)) != MQTTCLIENT_SUCCESS)
    {
        log_error("Failed to connect, return code %d\n", rc);
        return rc;
    }
    log_debug("subscribe to topic");
    MQTTClient_subscribe(*(sensorClient->mqttClient), TOPIC, QOS);


    return 0;
}

int SensorClient_stop(SensorManager_t *client) {

    log_debug("stop");
    Manager_t* sensorClient = *client;
    MQTTClient_unsubscribe(*(sensorClient->mqttClient), TOPIC);
    MQTTClient_disconnect(*(sensorClient->mqttClient),2000);
    return 0;
}

int SensorClient_destroy(SensorManager_t *client) {

    log_debug("free");
    Manager_t* sensorClient = *client;
    MQTTClient_destroy(sensorClient->mqttClient);
    free(sensorClient->mqttClient);
    free(sensorClient);
    *client = NULL;
    ///usr/bin/valgrind --tool=memcheck --xml=yes --xml-file=/tmp/valgrind --gen-suppressions=all --leak-check=full --leak-resolution=med --track-origins=yes /home/rap/iotivity/gardener/build/gardener
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