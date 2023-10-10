#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>
#include "cocodevicesdk/coco_device_api.h"
#include "cocostandard/coco_std_api.h"
#include "cocostandard/coco_std_data_illuminance_types.h"
#include "cocostandard/coco_std_data_network_config_types.h"


static double luxVal = 198.56;
static int32_t rssiVal = 4;

static coco_std_resource_attribute_info_t luxAttr = {
    NULL,
    0,
    "res-1",
    COCO_STD_CAP_ILLUMINANCE_MEASUREMENT,
    "illuminance",
    COCO_STD_ATTR_CURRENT_LUMINANCE_LUX,
    "lux",
    "illuminance lux",
    COCO_STD_DATA_TYPE_DOUBLE,
    0,
    &luxVal,
    &luxVal,
    &luxVal,
    &luxVal,
    0,
    10,
    &luxVal,
    1,
    0,
    0,
    1556539804
};

static coco_std_resource_attribute_info_t rssiAttr = {
    NULL,
    0,
    "res-1",
    COCO_STD_CAP_NETWORK_CONFIGURATION,
    "NetworkConfiguration",
    COCO_STD_ATTR_NW_CONFIG_RSSI,
    "rssi",
    "rssi",
    COCO_STD_DATA_TYPE_INT32,
    0,
    &rssiVal,
    &rssiVal,
    &rssiVal,
    &rssiVal,
    0,
    10,
    &rssiVal,
    1,
    0,
    0,
    1556539804
};

static coco_std_resource_capability_t capArr[2] = {
  {
    {
      NULL,
      0,
      "res-1",
      COCO_STD_CAP_ILLUMINANCE_MEASUREMENT,
      "illuminance",
      0,
      NULL,
      0,
      0
    },
    1,
    &luxAttr
  }, {
    {
      NULL,
      0,
      "res-1",
      COCO_STD_CAP_NETWORK_CONFIGURATION,
      "NetworkConfiguration",
      0,
      NULL,
      0,
      0
    },
    1,
    &rssiAttr
  }
};

static coco_std_resource_t resourceInfo = {
  {
    NULL,
    0,
    "res-1",
    "Illuminance Sensor",
    "Anchor",
    "V1",
    "1.0.0",
    "ILLUMINATION_SENSOR",
    COCO_STD_POWER_SRC_BATTERY,
    COCO_STD_RCVR_TYPE_WHEN_STIMULATED,
    COCO_STD_STATUS_SUCCESS,
    0,
    0
  },
  2,
  capArr
};

/*************************************************************************************
 *                          PRIVATE FUNCTIONS                                        *
 *************************************************************************************/

static inline void coco_device_logger_cb(const char *timestamp, int level, uint64_t tid,
                                         const char *funcName, int lineNum, const char *msg) {

  printf("coco_device_sdk: %s():%d: %s", funcName, lineNum, msg);
  fflush(stdout);
  return;
}

static inline void coco_device_join_nw_status_cb(int32_t status) {
  printf("App: coco_device_join_nw_status_cb() status: %d\n", status);
  return;
}

static inline void coco_device_add_res_status_cb(int32_t status, void *context) {
  printf("App: coco_device_add_res_status_cb() status: %d\n", status);
  return;
}

static inline void device_init_auth() {
  while (-1 == coco_device_init_auth()) {
    printf("App: Will re-try init auth after 3 seconds\n");
    sleep(3);
  }
}

static void device_init(char *cwd, char *configFilePath) {
  coco_device_init_params_t deviceInitParams = { 0 };

  deviceInitParams.cwdPath = cwd;
  deviceInitParams.configFilePath = configFilePath;
  deviceInitParams.downloadPath = cwd;
  deviceInitParams.tempPath = "/tmp/";
  deviceInitParams.coconetConnStatusCb = coco_device_join_nw_status_cb;
  deviceInitParams.addResStatusCb = coco_device_add_res_status_cb;
  deviceInitParams.firmwareVersion = "1.0.0";
  deviceInitParams.isExtendable = true;
  deviceInitParams.powerSource = COCO_STD_POWER_SRC_BATTERY;
  deviceInitParams.receiverType = COCO_STD_RCVR_TYPE_RX_ON_WHEN_IDLE;
  deviceInitParams.skipSSLVerification = 1;
  deviceInitParams.logLevel = LOG_DEBUG;
  deviceInitParams.loggerCb = coco_device_logger_cb;

  printf("App: Starting init\n");

  int retVal = coco_device_init(&deviceInitParams);

  if (-1 == retVal) {
    printf("App: coco_device_init failed\n");
  } else if (0 == retVal) {
    device_init_auth();
  } else {
    printf("App: Init Success\n");
  }
}

static double get_next_lux_val(double currVal) {
  double nextVal = 0.0;
  int whole, decimal, sign;

  // Generated signed number to be added to the current attribute value
  sign = rand() % 2;
  whole = (rand() % 3) + 1;
  decimal = (rand() % 100);
  if ((0 == sign && currVal >= 253) || currVal >= 498) {
    sign = -1;
  }
  else {
    sign = 1;
  }

  // Add number generated above to the current attribute value to get the new attribute value
  nextVal = currVal + (sign * ((double)whole + ((double)decimal/100)));
  printf("App: next lux val: %lf\n", nextVal);

  return nextVal;
}

static double get_next_rssi_val() {
  int nextVal;

  // Generated random number
  nextVal = (rand() % 4) + 1;
  printf("App: next rssi val: %d\n", nextVal);
  
  return nextVal;
}

int main(int argc, char *argv[]) {
  if (3 != argc) {
    printf("Usage: %s <cwd> <config path>\n", argv[0]);
    exit(1);
  }

  char *cwd = argv[1];
  char *configFilePath = argv[2];

  device_init(cwd, configFilePath);

  if (-1 == coco_device_add_resource(&resourceInfo, 1, 0, 0, NULL)) {
    printf("App: Add resource failed\n");
    exit(1);
  }

  // Run a loop to update the attribute value of the added resource every 2 seconds
  while(1) {
    printf("App: Update attribute\n");
    
    // Set next lux attribute value
    *(double *)(luxAttr.currentValue) = get_next_lux_val(*(double *)(luxAttr.currentValue));

    // Update the lux attribute value of the added resource using COCO device SDK API
    if (-1 == coco_device_resource_attribute_update(&luxAttr, NULL)) {
      printf("App: Update attribute failed\n");
      break;
    }

    // Set next rssi attribute value
    *(int32_t *)(rssiAttr.currentValue) = get_next_rssi_val();

    // Update the lux attribute value of the added resource using COCO device SDK API
    if (-1 == coco_device_resource_attribute_update(&rssiAttr, NULL)) {
      printf("App: Update attribute failed\n");
      break;
    }
    
    sleep(2);
  }

  sleep(10);

  return 0;
}
