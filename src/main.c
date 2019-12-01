/****************************************************************************
 *
 * Copyright 2019 Jozef Kralik All Rights Reserved.
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

#include "oc_api.h"
#include <signal.h>
#include <inttypes.h>
#include "serial.h"

static int quit;

#if defined(_WIN32)
#include <windows.h>

static CONDITION_VARIABLE cv;
static CRITICAL_SECTION cs;

static void
signal_event_loop(void)
{
  WakeConditionVariable(&cv);
}

void
handle_signal(int signal)
{
  signal_event_loop();
  quit = 1;
}

static int
init(void)
{
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);

  signal(SIGINT, handle_signal);
  return 0;
}

static void
run(void)
{
  while (quit != 1) {
    oc_clock_time_t next_event = oc_main_poll();
    if (next_event == 0) {
      SleepConditionVariableCS(&cv, &cs, INFINITE);
    } else {
      oc_clock_time_t now = oc_clock_time();
      if (now < next_event) {
        SleepConditionVariableCS(
          &cv, &cs, (DWORD)((next_event - now) * 1000 / OC_CLOCK_SECOND));
      }
    }
  }
}

#elif defined(__linux__) || defined(__ANDROID_API__)
#include <pthread.h>

static pthread_mutex_t mutex;
static pthread_cond_t cv;

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
    if (signal == SIGPIPE) {
        return;
    }
    signal_event_loop();
    quit = 1;
}

static int
init(void)
{
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);

    if (pthread_mutex_init(&mutex, NULL) < 0) {
        OC_ERR("pthread_mutex_init failed!");
        return -1;
    }
    return 0;
}

static void
run(void)
{
    while (quit != 1) {
        oc_clock_time_t next_event = oc_main_poll();
        pthread_mutex_lock(&mutex);
        if (next_event == 0) {
            pthread_cond_wait(&cv, &mutex);
        } else {
            struct timespec ts;
            ts.tv_sec = (next_event / OC_CLOCK_SECOND);
            ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
            pthread_cond_timedwait(&cv, &mutex, &ts);
        }
        pthread_mutex_unlock(&mutex);
    }
}

#else
#error "Unsupported OS"
#endif

// define application specific values.
static const char *spec_version = "ocf.1.0.0";
static const char *data_model_version = "ocf.res.1.0.0";

static const char *resource_rt = "core.switch.1";
static const char *device_rt = "oic.d.switch.device";
static const char *device_name = "Cloud Device";

static const char *manufacturer = "ocfcloud.com";

oc_resource_t *res1;

static void
cloud_status_handler(oc_cloud_context_t *ctx, oc_cloud_status_t status,
                     void *data)
{
    (void)data;
    PRINT("\nCloud Manager Status:\n");
    if (status & OC_CLOUD_REGISTERED) {
        PRINT("\t\t-Registered\n");
    }
    if (status & OC_CLOUD_TOKEN_EXPIRY) {
        PRINT("\t\t-Token Expiry: ");
        if (ctx) {
            PRINT("%d\n", oc_cloud_get_token_expiry(ctx));
        } else {
            PRINT("\n");
        }
    }
    if (status & OC_CLOUD_FAILURE) {
        PRINT("\t\t-Failure\n");
    }
    if (status & OC_CLOUD_LOGGED_IN) {
        PRINT("\t\t-Logged In\n");
    }
    if (status & OC_CLOUD_LOGGED_OUT) {
        PRINT("\t\t-Logged Out\n");
    }
    if (status & OC_CLOUD_DEREGISTERED) {
        PRINT("\t\t-DeRegistered\n");
    }
    if (status & OC_CLOUD_REFRESHED_TOKEN) {
        PRINT("\t\t-Refreshed Token\n");
    }
}

static int
app_init(void)
{
    int ret = oc_init_platform(manufacturer, NULL, NULL);
    ret |= oc_add_device("/oic/d", device_rt, device_name, spec_version,
                         data_model_version, NULL, NULL);
    return ret;
}

struct switch_t
{
    bool state;
};

struct switch_t switch1 = { 0 };

static void
get_handler(oc_request_t *request, oc_interface_mask_t iface, void *user_data)
{
    struct switch_t *light = (struct switch_t *)user_data;

    PRINT("get_handler:\n");

    oc_rep_start_root_object();
    switch (iface) {
        case OC_IF_BASELINE:
            oc_process_baseline_interface(request->resource);
            /* fall through */
        case OC_IF_RW:
            oc_rep_set_boolean(root, state, light->state);
            oc_rep_set_text_string(root, name, "Light");
            break;
        default:
            break;
    }
    oc_rep_end_root_object();
    oc_send_response(request, OC_STATUS_OK);
}

static void
post_handler(oc_request_t *request, oc_interface_mask_t iface_mask,
             void *user_data)
{
    struct switch_t *light = (struct switch_t *)user_data;
    (void)iface_mask;
    printf("post_handler:\n");
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
        char *key = oc_string(rep->name);
        printf("key: %s ", key);
        if (key && !strcmp(key, "state")) {
            switch (rep->type) {
                case OC_REP_BOOL:
                    light->state = rep->value.boolean;
                    PRINT("value: %d\n", light->state);
                    if (light->state) {
                        write_to_serial_port('1');
                    } else {
                        write_to_serial_port('0');
                    }
                    break;
                default:
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }
        rep = rep->next;
    }
    oc_send_response(request, OC_STATUS_CHANGED);
}

static void write_serial(char data){
    int i = write_to_serial_port(data);
    if (i == -1) {
        close_serial_port();
        init_serial_port();
        write_to_serial_port(data);
    }
}

static void
register_resources(void)
{
    res1 = oc_new_resource(NULL, "/switch/1", 1, 0);
    oc_resource_bind_resource_type(res1, resource_rt);
    oc_resource_bind_resource_interface(res1, OC_IF_RW);
    oc_resource_set_default_interface(res1, OC_IF_RW);
    oc_resource_set_discoverable(res1, true);
    oc_resource_set_observable(res1, true);
    oc_resource_set_request_handler(res1, OC_GET, get_handler, &switch1);
    oc_resource_set_request_handler(res1, OC_POST, post_handler, &switch1);
    oc_cloud_add_resource(res1);
    oc_add_resource(res1);
}

int
main(void)
{
    init_serial_port();
    int ret = init();
    if (ret < 0) {
        return ret;
    }

    static const oc_handler_t handler = { .init = app_init,
            .signal_event_loop = signal_event_loop,
            .register_resources =
            register_resources };
    oc_storage_config("./cloud_server_creds/");

    ret = oc_main_init(&handler);
    if (ret < 0)
        return ret;

    oc_cloud_context_t *ctx = oc_cloud_get_context(0);
    if (ctx) {
        oc_cloud_manager_start(ctx, cloud_status_handler, NULL);
    }

    run();

    oc_cloud_manager_stop(ctx);
    oc_main_shutdown();
    close_serial_port();
    return 0;
}