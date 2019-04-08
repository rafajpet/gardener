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

    int ret = fopen("IDoNotExist.txt", "w");
    perror("Opening IDoNotExist:");
    //...
    return 0;

//    pthread_t mqtt_subscribe;
//
//    int t_mqtt_id = pthread_create(&mqtt_subscribe, NULL, subscribe_function, NULL);
//    if(t_mqtt_id) {
//        log_error("unable to create thread");
//        exit(EXIT_FAILURE);
//    }
//
//
//    const struct Config config;
//    const char* file = "hello";
//    if(load_config(&config, file)){
//        log_debug("Unable to load config file");
//    } else {
//        log_debug("Load config file");
//    }
//
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

