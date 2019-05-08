/* getopt_long_demo - demonstrate getopt_long() usage
 *
 * This application shows you one way of using getopt_long()
 * to process your command-line options and store them in a
 * global structure for easy access.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "config.h"

/* doc2html supports the following command-line arguments:
 *
 * -I - don't produce a keyword index
 * -l lang - produce output in the specified language, lang
 * -o outfile - write output to outfile instead of stdout
 * -v - be verbose; more -v means more diagnostics
 * additional file names are used as input files
 *
 * In addition to the standard 'short' options, this
 * version also supports these 'long' options:
 *
 * --no-index
 * --language=lang
 * --output=outfile
 * --verbose
 *
 * The optString global tells getopt_long() which options we
 * support, and which options have arguments.
 *
 * The longOpts global tells getopt_long() which long options
 * we support, and which long options have arguments.
 */

struct globalArgs_t {
    int noIndex;				/* -I option */
    char *langCode;				/* -l option */
    const char *outFileName;	/* -o option */
    FILE *outFile;
    int verbosity;				/* -v option */
    char **inputFiles;			/* input files */
    int numInputFiles;			/* # of input files */
    int randomized;				/* --randomize option */
} globalArgs;

static const char *optString = "Il:o:vh?";

static const struct option longOpts[] = {
        { "no-index", no_argument, NULL, 'I' },
        { "language", required_argument, NULL, 'l' },
        { "output", required_argument, NULL, 'o' },
        { "verbose", no_argument, NULL, 'v' },
        { "randomize", no_argument, NULL, 0 },
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
};

/* Display program usage, and exit.
 */
void display_usage( void )
{
    puts( "doc2html - convert documents to HTML" );
    /* ... */
    exit( EXIT_FAILURE );
}

/* Convert the input files to HTML, governed by globalArgs.
 */
void convert_document( void )
{
    /* ... */
    printf( "no-index: %d\n", globalArgs.noIndex );
    printf( "language: %s\n", globalArgs.langCode );
    printf( "output: %s\n", globalArgs.outFileName );
    printf( "verbosity: %d\n", globalArgs.verbosity );
    printf( "randomized: %d\n", globalArgs.randomized );
}

int main( int argc, char *argv[] )
{
//    Config_t app_config;
//    Config_default(&app_config);
//    Config_set_parameter(&app_config, argc, argv);
//    Config_print(&app_config);



    int opt = 0;
    int longIndex = 0;

    globalArgs.randomized = 0;

    /* Process the arguments with getopt_long(), then
     * populate globalArgs.
     */
    opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    while( opt != -1 ) {
        switch( opt ) {
            case 'I':
                globalArgs.noIndex = 1;	/* true */
                break;

            case 'l':
                globalArgs.langCode = optarg;
                break;

            case 'o':
                globalArgs.outFileName = optarg;
                break;

            case 'v':
                globalArgs.verbosity++;
                break;

            case 'h':	/* fall-through is intentional */
            case '?':
                display_usage();
                break;

            case 0:		/* long option without a short arg */
                if( strcmp( "randomize", longOpts[longIndex].name ) == 0 ) {
                    globalArgs.randomized = 1;
                }
                break;

            default:
                /* You won't actually get here. */
                break;
        }

        opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    }

    globalArgs.inputFiles = argv + optind;
    globalArgs.numInputFiles = argc - optind;

    convert_document();

    return EXIT_SUCCESS;
}


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <pthread.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//
//
//// local
//#include "log.h"
//#include "subscriber.h"
//
//int message_receive(void *context, char *topicName, int topicLen, MQTTClient_message *message)
//{
//    int i;
//    char* payloadptr;
//
//    printf("Message arrived\n");
//    printf("     topic: %s\n", topicName);
//    printf("   message: ");
//
//    payloadptr = message->payload;
//    for(i=0; i<message->payloadlen; i++)
//    {
//        putchar(*payloadptr++);
//    }
//    putchar('\n');
//    MQTTClient_freeMessage(&message);
//    MQTTClient_free(topicName);
//    return 1;
//}
//
//
//int main(void){
//
//    SensorManager_t  client;
//    SensorManagerConfig_t clientConfig;
//    strcpy(clientConfig.topic, "test");
//    strcpy(clientConfig.address, "tcp://localhost:1883");
//
//    SensorManager_init(&client, &clientConfig);
//    SensorManager_set_receive(&client, message_receive);
//    SensorManager_start(&client);
//
//    puts("Press any key to stop...");
//    getchar();
//    log_debug("This is end");
//
////    int t_mqtt_id = pthread_create(&mqtt_subscribe, NULL, subscribe_function, NULL);
////    if(t_mqtt_id) {
////        log_error("unable to create thread");
////        exit(EXIT_FAILURE);
////    }
//    SensorManager_stop(&client);
//    SensorManager_destroy(&client);
//
//
////    int ch;
////    do
////    {
////        ch = getchar();
////    } while(ch!='q');
////    stop_subscribe();
////    pthread_join(mqtt_subscribe, NULL);
////    log_info("program exit");
//    return EXIT_SUCCESS;
//}
//
