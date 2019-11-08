
/****************************************************************************
Copyright (c) 2019 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_downloadable_capabilities_config_def.h

DESCRIPTION 
    This file contains the module specific configuration structure(s) and
    configuration block id(s) to be used with the config_store library.

*/

/*********************************/
/* GENERATED FILE - DO NOT EDIT! */
/*********************************/

#ifndef _SINK_DOWNLOADABLE_CAPABILITIES_CONFIG_DEF_H_
#define _SINK_DOWNLOADABLE_CAPABILITIES_CONFIG_DEF_H_

#include "config_definition.h"

#define DOWNLOADABLE_CAPS_CONFIG_BLK_ID 1490

typedef struct {
    unsigned short filename_1[25];
    unsigned short filename_2[25];
    unsigned short filename_3[25];
    unsigned short filename_4[25];
    unsigned short filename_5[25];
    unsigned short applicable_processor_type_4:4;
    unsigned short applicable_processor_type_3:4;
    unsigned short applicable_processor_type_2:4;
    unsigned short applicable_processor_type_1:4;
    unsigned short padding:12;
    unsigned short applicable_processor_type_5:4;
} downloadable_caps_config_def_t;

#endif /* _SINK_DOWNLOADABLE_CAPABILITIES_CONFIG_DEF_H_ */
