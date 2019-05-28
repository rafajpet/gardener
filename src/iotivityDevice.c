//
// Created by rafik on 19.5.2019.
//

#include "iotivityDevice.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>

#include "iotivity-constrained/cloud_access.h"
#include "iotivity-constrained/oc_api.h"
#include "iotivity-constrained/oc_core_res.h"
#include "iotivity-constrained/port/oc_clock.h"
#include "iotivity-constrained/rd_client.h"


static int
app_init(void)
{
    int ret = oc_init_platform(manufacturer, NULL, NULL);
    ret |= oc_add_device("/oic/d", device_rt, device_name, spec_version,
                         data_model_version, NULL, NULL);
    return ret;
}


int IotivityDevice_init(IotivityDevice_t *iotivityDevice, IotivityConfig_t *config) {
    return -1;
}

int IotivityDevice_set_device(IotivityDevice_t *iotivityDevice, const char *device_rt, const char *device_name, const char *manufacturer) {
    return -1;
}

int IotivityDevice_register_resource() {
    return -1;
}

int IotivityDevice_start() {
    return -1;
}

int IotivityDevice_destroy(){
    return -1;
}