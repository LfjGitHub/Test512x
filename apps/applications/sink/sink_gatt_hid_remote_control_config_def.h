
/****************************************************************************
Copyright (c) 2019 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_gatt_hid_remote_control_config_def.h

DESCRIPTION 
    This file contains the module specific configuration structure(s) and
    configuration block id(s) to be used with the config_store library.

*/

/*********************************/
/* GENERATED FILE - DO NOT EDIT! */
/*********************************/

#ifndef _SINK_GATT_HID_REMOTE_CONTROL_CONFIG_DEF_H_
#define _SINK_GATT_HID_REMOTE_CONTROL_CONFIG_DEF_H_

#include "config_definition.h"

typedef struct {
    unsigned short input_id:4;
    unsigned short hid_code:12;
} gattHidRcLookupTable_t;

#define SINK_GATT_HIDRC_READONLY_CONFIG_BLK_ID 900

typedef struct {
    gattHidRcLookupTable_t lookup_table[1];
} sink_gatt_hidrc_readonly_config_def_t;

#endif /* _SINK_GATT_HID_REMOTE_CONTROL_CONFIG_DEF_H_ */
