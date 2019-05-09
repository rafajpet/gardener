//
// Created by rafik on 8.5.2019.
//
//system
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

//local
#include "config.h"
#include "log.h"

#define MQTT_ADDRESS_PARAM "mqtt_host"
#define MQTT_PORT_PARAM  "mqtt_port"
#define MQTT_TOPIC_PARAM "mqtt_topic"
#define TLS_ENABLE "tls"
#define MQTT_CERT_FILE_PARAM "mqtt_cert"
#define MQTT_CERT_FILE_KEY_PARAM "mqtt_cert_key"
#define IOTIVITY_ENDPOINT_PARAM "iotivity_endpoint"
#define IOTIVITY_STORAGE_PATH_PARAM "iotivity_path"


static const struct option options[] = {
        { MQTT_ADDRESS_PARAM, required_argument, NULL, 0 },
        { MQTT_PORT_PARAM, required_argument, NULL, 1 },
        { TLS_ENABLE, no_argument, NULL, 2 },
        { MQTT_CERT_FILE_PARAM, required_argument, NULL, 3 },
        { MQTT_CERT_FILE_KEY_PARAM, required_argument, NULL, 4 },
        { IOTIVITY_ENDPOINT_PARAM, required_argument, NULL, 5 },
        { IOTIVITY_STORAGE_PATH_PARAM, required_argument, NULL, 6 },
        { MQTT_TOPIC_PARAM, required_argument, NULL, 7 },
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
};



static void print_help(){
    printf("\nGardener is application connect mqtt sensors with iotivity lite \n \n");

    printf("PARAMETERS: \n");
    printf("--%s HOST  configuration mqtt broker address \n", MQTT_ADDRESS_PARAM);
    printf("--%s PORT  mqtt port \n", MQTT_PORT_PARAM);
    printf("--%s TOPIC  mqtt topic to subscribe REQUIRED\n", MQTT_TOPIC_PARAM);
    printf("--%s  enable tls communication between mqtt broker \n", TLS_ENABLE);
    printf("--%s FILE  for loading mqtt public certificate \n", MQTT_CERT_FILE_PARAM);
    printf("--%s FILE  for loading mqtt private key \n", MQTT_CERT_FILE_KEY_PARAM);
    printf("--%s HOST  address of OCF cloud in format: coap+tcp://HOST:PORT \n", IOTIVITY_ENDPOINT_PARAM);
    printf("--%s PATH  for iotivity internal configuration \n", IOTIVITY_STORAGE_PATH_PARAM);

}

static bool isValid(Config_t *config) {
    if(strlen(config->mqtt_topic) == 0){
        log_error("%s parameter is required", MQTT_TOPIC_PARAM);
        return false;
    }
    return true;
}


int Config_default(Config_t *config) {
    if(!config){
        return CONFIG_INVALID_INPUT;
    }
    strcpy(config->mqtt_address, DEFAULT_MQTT_ADDRESS);
    config->mqtt_port = DEFAULT_MQTT_PORT;
    config->tls_enable = false;
    strcpy(config->mqtt_cert_file, DEFAULT_MQTT_CERT_FILE);
    strcpy(config->mqtt_cert_key_file, DEFAULT_MQTT_CERT_KEY_FILE);
    strcpy(config->iotivity_endpoint, DEFAULT_IOTIVITY_ENDPOINT);
    strcpy(config->iotivity_storage_path, DEFAULT_IOTIVITY_STORAGE_PATH);

    return CONFIG_SUCCESS;
}

void Config_print(Config_t *config) {
    log_debug("\n Configuration: \n MQTT_ADDRESS: %s \n MQTT_PORT: %d \n MQTT_TOPIC: %d \n TLS_ENABLE: %d \n MQTT_CERT_FILE_PATH: %s \n "
              "MQTT_CERT_KEY_FILE_PATH: %s \n IOTIVITY_ENDPOINT: %s \n IOTIVITY_PATH: %s \n"
            ,config->mqtt_address, config->mqtt_port, config->mqtt_topic, config->tls_enable, config->mqtt_cert_file
            ,config->mqtt_cert_key_file, config->iotivity_endpoint, config->iotivity_storage_path);
}

int Config_set_parameter(Config_t *config, int argc, char *argv[]) {

    if(!config){
        log_error("invalid input");
        return CONFIG_INVALID_INPUT;
    }

    int opt = 0;
    int longIndex = 0;
    bool help = false;
    opt = getopt_long( argc, argv, "h", options, &longIndex );
    while( opt != -1 ) {
        switch( opt ) {
            case 'h':
                help = true;
                break;
            case 0:
                if( strcmp( MQTT_ADDRESS_PARAM, options[longIndex].name ) == 0 ) {
                    strcpy(config->mqtt_address, optarg);
                }
                break;
            case 1:
                if( strcmp( MQTT_PORT_PARAM, options[longIndex].name ) == 0 ) {
                    int port = (int) strtol(optarg, (char **)NULL, 10);
                    config->mqtt_port = port;
                }
                break;
            case 2:
                if( strcmp( TLS_ENABLE, options[longIndex].name ) == 0 ) {
                    config->tls_enable = true;
                }
                break;
            case 3:
                if( strcmp( MQTT_CERT_FILE_PARAM, options[longIndex].name ) == 0 ) {
                    strcpy(config->mqtt_cert_file, optarg);
                }
                break;
            case 4:
                if( strcmp( MQTT_CERT_FILE_KEY_PARAM, options[longIndex].name ) == 0 ) {
                    strcpy(config->mqtt_cert_key_file, optarg);
                }
                break;
            case 5:
                if( strcmp( IOTIVITY_ENDPOINT_PARAM, options[longIndex].name ) == 0 ) {
                    strcpy(config->iotivity_endpoint, optarg);
                }
                break;
            case 6:
                if( strcmp( IOTIVITY_STORAGE_PATH_PARAM, options[longIndex].name ) == 0 ) {
                    strcpy(config->iotivity_storage_path, optarg);
                }
                break;
            case 7:
                if( strcmp( MQTT_TOPIC_PARAM, options[longIndex].name ) == 0 ) {
                    strcpy(config->mqtt_topic, optarg);
                }
                break;
            default:
                /* You won't actually get here. */
                break;
        }

        opt = getopt_long( argc, argv, "h", options, &longIndex );
    }

    if(help){
        print_help();
        exit(EXIT_SUCCESS);
    }

    if(!isValid(config)){
        print_help();
        return CONFIG_INVALID_INPUT;
    }

    return CONFIG_SUCCESS;
}

int Config_get_mqtt_address(Config_t *config, char *address) {
    const char* tcp_prefix = "tcp://";
    char port[8];
    sprintf(port, ":%d", config->mqtt_port);
    int size = strlen(tcp_prefix) + strlen(port) + strlen(config->mqtt_address) + 1;
    memcpy(address, tcp_prefix, strlen(tcp_prefix));
    memcpy(address + strlen(tcp_prefix), config->mqtt_address, strlen(config->mqtt_address));
    memcpy(address + strlen(tcp_prefix) + strlen(config->mqtt_address), port, strlen(port));
    address[size - 1] = '\0';
    return OK;
}



//struct globalArgs_t {
//    int noIndex;				/* -I option */
//    char *langCode;				/* -l option */
//    const char *outFileName;	/* -o option */
//    FILE *outFile;
//    int verbosity;				/* -v option */
//    char **inputFiles;			/* input files */
//    int numInputFiles;			/* # of input files */
//    int randomized;				/* --randomize option */
//} globalArgs;
//
//static const char *optString = "Il:o:vh?";
//
//static const struct option longOpts[] = {
//        { "no-index", no_argument, NULL, 'I' },
//        { "language", required_argument, NULL, 'l' },
//        { "output", required_argument, NULL, 'o' },
//        { "verbose", no_argument, NULL, 'v' },
//        { "randomize", no_argument, NULL, 0 },
//        { "help", no_argument, NULL, 'h' },
//        { NULL, no_argument, NULL, 0 }
//};
//
///* Display program usage, and exit.
// */
//void display_usage( void )
//{
//    puts( "doc2html - convert documents to HTML" );
//    /* ... */
//    exit( EXIT_FAILURE );
//}
//
///* Convert the input files to HTML, governed by globalArgs.
// */
//void convert_document( void )
//{
//    /* ... */
//    printf( "no-index: %d\n", globalArgs.noIndex );
//    printf( "language: %s\n", globalArgs.langCode );
//    printf( "output: %s\n", globalArgs.outFileName );
//    printf( "verbosity: %d\n", globalArgs.verbosity );
//    printf( "randomized: %d\n", globalArgs.randomized );
//}
//
//int main( int argc, char *argv[] )
//{
////    Config_t app_config;
////    Config_default(&app_config);
////    Config_set_parameter(&app_config, argc, argv);
////    Config_print(&app_config);
//
//
//
//    int opt = 0;
//    int longIndex = 0;
//
//    globalArgs.randomized = 0;
//
//    /* Process the arguments with getopt_long(), then
//     * populate globalArgs.
//     */
//    opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
//    while( opt != -1 ) {
//        switch( opt ) {
//            case 'I':
//                globalArgs.noIndex = 1;	/* true */
//                break;
//
//            case 'l':
//                globalArgs.langCode = optarg;
//                break;
//
//            case 'o':
//                globalArgs.outFileName = optarg;
//                break;
//
//            case 'v':
//                globalArgs.verbosity++;
//                break;
//
//            case 'h':	/* fall-through is intentional */
//            case '?':
//                display_usage();
//                break;
//
//            case 0:		/* long option without a short arg */
//                if( strcmp( "randomize", longOpts[longIndex].name ) == 0 ) {
//                    globalArgs.randomized = 1;
//                }
//                break;
//
//            default:
//                /* You won't actually get here. */
//                break;
//        }
//
//        opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
//    }
//
//    globalArgs.inputFiles = argv + optind;
//    globalArgs.numInputFiles = argc - optind;
//
//    convert_document();
//
//    return EXIT_SUCCESS;
//}

