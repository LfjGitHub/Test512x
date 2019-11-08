
/****************************************************************************
Copyright (c) 2019 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_inputmanager_config_def.h

DESCRIPTION 
    This file contains the module specific configuration structure(s) and
    configuration block id(s) to be used with the config_store library.

*/

/*********************************/
/* GENERATED FILE - DO NOT EDIT! */
/*********************************/

#ifndef _SINK_INPUTMANAGER_CONFIG_DEF_H_
#define _SINK_INPUTMANAGER_CONFIG_DEF_H_

#include "config_definition.h"

typedef struct {
    unsigned short mask;
    unsigned short state_mask;
    unsigned short user_event:8;
    unsigned short input_event:8;
} eventLookupTable_t;

typedef struct {
    unsigned short multipleDetectTimer;
    unsigned short shortTimer;
    unsigned short longTimer;
    unsigned short vLongTimer;
    unsigned short vvLongTimer;
    unsigned short repeatTimer;
} timerConfig_t;

#define SINK_INPUTMANAGER_READONLY_CONFIG_BLK_ID 914

typedef struct {
    timerConfig_t input_timers;
    eventLookupTable_t lookuptable[1];
} sink_inputmanager_readonly_config_def_t;

#endif /* _SINK_INPUTMANAGER_CONFIG_DEF_H_ */
