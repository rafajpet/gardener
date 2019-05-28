/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include "iotivity-constrained/cloud_access.h"
#include "iotivity-constrained/oc_api.h"
#include "iotivity-constrained/oc_core_res.h"
#include "iotivity-constrained/port/oc_clock.h"
#include "iotivity-constrained/rd_client.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

// define application specific values.
static const char *spec_version = "ocf.1.0.0";
static const char *data_model_version = "ocf.res.1.0.0";

static const char *resource_rt = "core.light";
static const char *device_rt = "oic.d.cloudDevice";
static const char *device_name = "Cloud Device";

static const char *manufacturer = "ocfcloud.com";

pthread_mutex_t mutex;
pthread_cond_t cv;
struct timespec ts;

pthread_mutex_t app_mutex;
oc_resource_t *res1;
oc_resource_t *res2;
oc_link_t *link1;
oc_link_t *link2;
oc_link_t *lresd;
oc_link_t *lresp;

int quit = 0;

#define MAX_URI_LENGTH (30)
static char a_light[MAX_URI_LENGTH];

static oc_string_t uid;
static oc_string_t access_token;

#define OC_IPV6_ADDRSTRLEN (46)
static char address[OC_IPV6_ADDRSTRLEN + 8];
static oc_endpoint_t set_ep;

static int
app_init(void)
{
    int ret = oc_init_platform(manufacturer, NULL, NULL);
    ret |= oc_add_device("/oic/d", device_rt, device_name, spec_version,
                         data_model_version, NULL, NULL);
    return ret;
}

struct light_t {
    bool state;
    int power;
};

struct light_t light1 = {0};
struct light_t light2 = {0};

static void
get_handler(oc_request_t *request, oc_interface_mask_t interface,
            void *user_data)
{
    struct light_t* light = (struct light_t*) user_data;

    printf("get_handler:\n");

    oc_rep_start_root_object();
    switch (interface) {
        case OC_IF_BASELINE:
            oc_process_baseline_interface(request->resource);
            /* fall through */
        case OC_IF_RW:
            oc_rep_set_boolean(root, state, light->state);
            oc_rep_set_int(root, power, light->power);
            oc_rep_set_text_string(root, name, "Light");
            break;
        default:
            break;
    }
    oc_rep_end_root_object();
    oc_send_response(request, OC_STATUS_OK);
}

static void
post_handler(oc_request_t *request, oc_interface_mask_t iface_mask, void *user_data)
{
    struct light_t* light = (struct light_t*) user_data;
    (void)iface_mask;
    printf("post_handler:\n");
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
        char* key = oc_string(rep->name);
        printf("key: %s ", key);
        if (key && !strcmp(key, "state")) {
            switch (rep->type) {
                case OC_REP_BOOL:
                    light->state = rep->value.boolean;
                    printf("value: %d\n", light->state);
                    break;
                default:
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        } else if (key && !strcmp(key, "power")) {
            switch (rep->type) {
                case OC_REP_INT:
                    light->power = rep->value.integer;
                    printf("value: %d\n", light->power);
                    break;
                default:
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }
        rep = rep->next;
    }
    oc_send_response(request, OC_STATUS_CHANGED);
    oc_notify_observers(request->resource);
}

static void
parse_payload(oc_client_response_t *data)
{
    oc_rep_t *rep = data->payload;
    while (rep != NULL) {
        printf("key %s, value ", oc_string(rep->name));
        switch (rep->type) {
            case OC_REP_BOOL:
                printf("%d\n", rep->value.boolean);
                break;
            case OC_REP_INT:
                printf("%d\n", rep->value.integer);
                break;
            case OC_REP_STRING:
                printf("%s\n", oc_string(rep->value.string));
                if (strncmp("uid", oc_string(rep->name), oc_string_len(rep->name)) == 0) {
                    if (oc_string_len(uid))
                        oc_free_string(&uid);
                    oc_new_string(&uid, oc_string(rep->value.string),
                                  oc_string_len(rep->value.string));
                } else if (strncmp("accesstoken", oc_string(rep->name),
                                   oc_string_len(rep->name)) == 0) {
                    if (oc_string_len(access_token))
                        oc_free_string(&access_token);
                    oc_new_string(&access_token, oc_string(rep->value.string),
                                  oc_string_len(rep->value.string));
                }
                break;
            default:
                printf("NULL\n");
                break;
        }
        rep = rep->next;
    }
}

static void
post_response(oc_client_response_t *data)
{
    if (data->code == OC_STATUS_CHANGED)
        printf("POST response: CHANGED\n");
    else if (data->code == OC_STATUS_CREATED)
        printf("POST response: CREATED\n");
    else
        printf("POST response code %d\n", data->code);

    parse_payload(data);
}

static void
delete_response(oc_client_response_t *data)
{
    if (data->code == OC_STATUS_DELETED)
        printf("DELETE response: DELETED\n");
    else if (data->code == OC_STATUS_BAD_REQUEST)
        printf("DELETE response: BAD REQUEST\n");
    else
        printf("DELETE response code %d\n", data->code);
}

static oc_discovery_flags_t
discovery_handler(const char *anchor, const char *uri, oc_string_array_t types,
                  oc_interface_mask_t interfaces, oc_endpoint_t *endpoint,
                  oc_resource_properties_t bm, void *user_data)
{
    oc_discovery_flags_t ret = OC_CONTINUE_DISCOVERY;

    (void)anchor;
    (void)user_data;
    (void)interfaces;
    (void)bm;
    int i;
    int uri_len = strlen(uri);
    uri_len = (uri_len >= MAX_URI_LENGTH) ? MAX_URI_LENGTH - 1 : uri_len;
    for (i = 0; i < (int)oc_string_array_get_allocated_size(types); i++) {
        char *t = oc_string_array_get_item(types, i);
        if (strlen(t) == 10 && strncmp(t, resource_rt, 10) == 0) {
            strncpy(a_light, uri, uri_len);
            a_light[uri_len] = '\0';

            printf("Resource %s hosted at endpoints:\n", a_light);
            oc_endpoint_t *ep = endpoint;
            while (ep != NULL) {
                PRINTipaddr(*ep);
                PRINT("\n");
                ep = ep->next;
            }
            ret = OC_STOP_DISCOVERY;
            goto exit;
        }
    }

    exit:
    oc_free_server_endpoints(endpoint);
    return ret;
}

static void
register_resources(void)
{
    res1 = oc_new_resource(NULL, "/light/1", 1, 0);
    oc_resource_bind_resource_type(res1, resource_rt);
    oc_resource_bind_resource_interface(res1, OC_IF_RW);
    oc_resource_set_default_interface(res1, OC_IF_RW);
    oc_resource_set_discoverable(res1, true);
    oc_resource_set_observable(res1, true);
    oc_resource_set_request_handler(res1, OC_GET, get_handler, &light1);
    oc_resource_set_request_handler(res1, OC_POST, post_handler, &light1);
    oc_add_resource(res1);

    res2 = oc_new_resource(NULL, "/light/2", 1, 0);
    oc_resource_bind_resource_type(res2, resource_rt);
    oc_resource_bind_resource_interface(res2, OC_IF_RW);
    oc_resource_set_default_interface(res2, OC_IF_RW);
    oc_resource_set_discoverable(res2, true);
    oc_resource_set_observable(res2, true);
    oc_resource_set_request_handler(res2, OC_GET, get_handler, &light2);
    oc_resource_set_request_handler(res2, OC_POST, post_handler, &light2);
    oc_add_resource(res2);
}

static void
signal_event_loop(void)
{
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mutex);
}

void
handle_signal(int signal)
{
    (void)signal;
    signal_event_loop();
    quit = 1;
}

static void *
process_func(void *data)
{
    (void)data;
    oc_clock_time_t next_event;

    while (quit != 1) {
        pthread_mutex_lock(&app_mutex);
        next_event = oc_main_poll();
        pthread_mutex_unlock(&app_mutex);
        pthread_mutex_lock(&mutex);
        if (next_event == 0) {
            pthread_cond_wait(&cv, &mutex);
        } else {
            ts.tv_sec = (next_event / OC_CLOCK_SECOND);
            ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
            pthread_cond_timedwait(&cv, &mutex, &ts);
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(0);
}

void
print_menu(void)
{
    pthread_mutex_lock(&app_mutex);
    printf("=====================================\n");
    printf("1. Sign-Up\n");
    printf("2. Sign-In\n");
    printf("3. Sign-Out\n");
    printf("4. Publish Resource\n");
    printf("5. Delete Resource\n");
    printf("6. Discovery Resource\n");
    printf("0. Quit\n");
    printf("=====================================\n");
    pthread_mutex_unlock(&app_mutex);
}

int
main(void)
{
    int init;
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);

    printf("set cloud address(ex. coaps+tcp://devices.ocfcloud.com:5684): ");
    strcpy(address, "coap+tcp://192.168.100.19:5684");

    oc_string_t address_str;
    oc_new_string(&address_str, address, strlen(address));

    oc_string_to_endpoint(&address_str, &set_ep, NULL);
    oc_free_string(&address_str);

    static const oc_handler_t handler = {.init = app_init,
            .signal_event_loop = signal_event_loop,
            .register_resources = register_resources };

#ifdef OC_SECURITY
    oc_storage_config("./cloud_linux_creds");
#endif /* OC_SECURITY */

    if (pthread_mutex_init(&mutex, NULL) < 0) {
        printf("pthread_mutex_init failed!\n");
        return -1;
    }

    if (pthread_mutex_init(&app_mutex, NULL) < 0) {
        printf("pthread_mutex_init failed!\n");
        return -1;
    }

    init = oc_main_init(&handler);
    if (init < 0)
        return init;

    pthread_t thread;
    if (pthread_create(&thread, NULL, process_func, NULL) != 0) {
        printf("Failed to create main thread\n");
        return -1;
    }

    char key[10];
    while (quit != 1) {
        print_menu();
        fflush(stdin);
        if (!scanf("%s", key)) {
            printf("scanf failed!!!!\n");
            quit = 1;
            handle_signal(0);
            break;
        }

        pthread_mutex_lock(&app_mutex);
        switch (key[0]) {
            case '1': {
                char auth_code[21];
                printf("set auth code(ex. j1o2j3a4e5h6o7n8g9): ");
                if (scanf("%20s", auth_code)) {
                    printf("auth_code: %s\n", auth_code);
                }
                oc_sign_up_with_auth(&set_ep, "ocfcloud.com", auth_code, 0, post_response,
                                     NULL);
                break;
            }
            case '2':
                oc_sign_in(&set_ep, oc_string(uid), oc_string(access_token), 0,
                           post_response, NULL);
                break;
            case '3':
                oc_sign_out(&set_ep, oc_string(access_token), 0, post_response, NULL);
                break;
            case '4':
                if (!link1) {
                    link1 = oc_new_link(res1);
                    link2 = oc_new_link(res2);
                    lresd = oc_new_link(oc_core_get_resource_by_index(OCF_D, 0));
                    lresp = oc_new_link(oc_core_get_resource_by_index(OCF_P, 0));
                    oc_list_add((oc_list_t)link1, link2);
                    oc_list_add((oc_list_t)link1, lresd);
                    oc_list_add((oc_list_t)link1, lresp);
                }
                rd_publish(&set_ep, link1, 0, post_response, LOW_QOS, NULL);
                break;
            case '5':
                rd_delete(&set_ep, NULL, 0, delete_response, LOW_QOS, NULL);
                break;
            case '6':
                oc_do_ip_discovery_at_endpoint(resource_rt, &discovery_handler, &set_ep,
                                               NULL);
                break;
            case '0':
                quit = 1;
                handle_signal(0);
                break;
            default:
                printf("unsupported command.\n");
                break;
        }
        pthread_mutex_unlock(&app_mutex);
    }

    if (oc_string_len(uid))
        oc_free_string(&uid);
    if (oc_string_len(access_token))
        oc_free_string(&access_token);
    oc_delete_link(link1);
    oc_delete_link(link2);
    oc_delete_link(lresp);
    oc_delete_link(lresd);

    pthread_join(thread, NULL);
    printf("pthread_join finish!\n");

    oc_main_shutdown();

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&app_mutex);
    return 0;
}

//#include <unistd.h>
//#include <stdio.h>
//#include <sys/socket.h>
//#include <stdlib.h>
//#include <netinet/in.h>
//#include <string.h>
//#define PORT 8080
//int main(int argc, char const *argv[])
//{
//    int server_fd, new_socket, valread;
//    struct sockaddr_in address;
//    int opt = 1;
//    int addrlen = sizeof(address);
//    char buffer[1024] = {0};
//    char *hello = "Hello from server";
//
//    // Creating socket file descriptor
//    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//    {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//    // Forcefully attaching socket to the port 8080
//    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
//                   &opt, sizeof(opt)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons( PORT );
//
//    // Forcefully attaching socket to the port 8080
//    if (bind(server_fd, (struct sockaddr *)&address,
//             sizeof(address))<0)
//    {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//    if (listen(server_fd, 3) < 0)
//    {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
//                             (socklen_t*)&addrlen))<0)
//    {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//    valread = read( new_socket , buffer, 1024);
//    printf("%s\n",buffer );
//    send(new_socket , hello , strlen(hello) , 0 );
//    printf("Hello message sent\n");
//    return 0;
//}
//#include <unistd.h>
//#include <stdio.h>
//#include <sys/socket.h>
//#include <stdlib.h>
//#include <netinet/in.h>
//#include <string.h>
//#define PORT 8080
//int main(int argc, char const *argv[])
//{
//    int server_fd, new_socket, valread;
//    struct sockaddr_in address;
//    int opt = 1;
//    int addrlen = sizeof(address);
//    char buffer[1024] = {0};
//    char *hello = "Hello from server";
//
//    // Creating socket file descriptor
//    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//    {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//    // Forcefully attaching socket to the port 8080
//    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
//                   &opt, sizeof(opt)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons( PORT );
//
//    // Forcefully attaching socket to the port 8080
//    if (bind(server_fd, (struct sockaddr *)&address,
//             sizeof(address))<0)
//    {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//    if (listen(server_fd, 3) < 0)
//    {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
//                             (socklen_t*)&addrlen))<0)
//    {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//    valread = read( new_socket , buffer, 1024);
//    printf("%s\n",buffer );
//    send(new_socket , hello , strlen(hello) , 0 );
//    printf("Hello message sent\n");
//    return 0;
//}






//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <getopt.h>
//#include <string.h>
//
//#include "config.h"
//#include "log.h"
//#include "subscriber.h"
//
//
//static int message_receive(void *context, char *topicName, int topicLen, MQTTClient_message *message);
//
//int main( int argc, char *argv[] )
//{
//    //config application
//    Config_t app_config;
//    Config_default(&app_config);
//    if(Config_set_parameter(&app_config, argc, argv)){
//        exit(EXIT_FAILURE);
//    }
//    SensorManager_t  client;
//    SensorManagerConfig_t clientConfig;
//    strcpy(clientConfig.topic, app_config.mqtt_topic);
//
//    char mqtt_address[100];
//    Config_get_mqtt_address(&app_config, mqtt_address);
//    strcpy(clientConfig.address, mqtt_address);
//
//    SensorManager_init(&client, &clientConfig);
//    SensorManager_set_receive(&client, message_receive);
//    SensorManager_start(&client);
//
//
//
//
//
//    puts("Press any key to stop...");
//    getchar();
//    log_debug("This is end");
//    SensorManager_stop(&client);
//    SensorManager_destroy(&client);
//
//    return EXIT_SUCCESS;
//}
//
//
//static int message_receive(void *context, char *topicName, int topicLen, MQTTClient_message *message)
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
