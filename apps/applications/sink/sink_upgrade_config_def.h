
/****************************************************************************
Copyright (c) 2019 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_upgrade_config_def.h

DESCRIPTION 
    This file contains the module specific configuration structure(s) and
    configuration block id(s) to be used with the config_store library.

*/

/*********************************/
/* GENERATED FILE - DO NOT EDIT! */
/*********************************/

#ifndef _SINK_UPGRADE_CONFIG_DEF_H_
#define _SINK_UPGRADE_CONFIG_DEF_H_

#include "config_definition.h"

typedef struct {
    unsigned short physical_partition_2:8;
    unsigned short physical_partition_1:8;
    unsigned short padding:12;
    unsigned short logical_type:4;
} logical_partition_pattern_t;

typedef struct {
    unsigned short padding:14;
    unsigned short protect_audio_during_upgrade:1;
    unsigned short enable_app_config_reset:1;
} upgrade_config_t;

#define SINK_UPGRADE_READONLY_CONFIG_BLK_ID 1378

typedef struct {
    upgrade_config_t upgrade_config;
    logical_partition_pattern_t logical_partitions_array[1];
} sink_upgrade_readonly_config_def_t;

#endif /* _SINK_UPGRADE_CONFIG_DEF_H_ */
