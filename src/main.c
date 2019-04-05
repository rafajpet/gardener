#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// local
#include "config.h"
#include "log.h"

int main(void){

    const struct Config config;
    const char *file = "hello";
    if(load_config(&config, file)){
        log_debug("Unable to load config file");
    } else {
        log_debug("Load config file");
    }

    return 0;
}