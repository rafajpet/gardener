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


int main(void){

    SensorClient  client;
    SensorClientConfig clientConfig;

    SensorClient_init(&client, &clientConfig);
    SensorClient_start(&client);
    SensorClient_stop(&client);
    SensorClient_free(&client);




//    pthread_t mqtt_subscribe;
//
//    int t_mqtt_id = pthread_create(&mqtt_subscribe, NULL, subscribe_function, NULL);
//    if(t_mqtt_id) {
//        log_error("unable to create thread");
//        exit(EXIT_FAILURE);
//    }



//    int ch;
//    do
//    {
//        ch = getchar();
//    } while(ch!='q');
//    stop_subscribe();
//    pthread_join(mqtt_subscribe, NULL);
//    log_info("program exit");
    return 0;
}

