#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>



// local
#include "config.h"
#include "log.h"
#include "subscriber.h"

int message_receive(void *context, char *topicName, int topicLen, MQTTClient_message *message)
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


int main(void){

    SensorManager_t  client;
    SensorManagerConfig_t clientConfig;
    strcpy(clientConfig.topic, "test");
    strcpy(clientConfig.address, "tcp://localhost:1883");

    SensorManager_init(&client, &clientConfig);
    SensorManager_set_receive(&client, message_receive);
    SensorManager_start(&client);

    puts("Press any key to stop...");
    getchar();
    log_debug("This is end");

//    int t_mqtt_id = pthread_create(&mqtt_subscribe, NULL, subscribe_function, NULL);
//    if(t_mqtt_id) {
//        log_error("unable to create thread");
//        exit(EXIT_FAILURE);
//    }
    SensorManager_stop(&client);
    SensorManager_destroy(&client);


//    int ch;
//    do
//    {
//        ch = getchar();
//    } while(ch!='q');
//    stop_subscribe();
//    pthread_join(mqtt_subscribe, NULL);
//    log_info("program exit");
    return EXIT_SUCCESS;
}

