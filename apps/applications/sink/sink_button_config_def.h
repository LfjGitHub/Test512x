
/****************************************************************************
Copyright (c) 2019 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_button_config_def.h

DESCRIPTION 
    This file contains the module specific configuration structure(s) and
    configuration block id(s) to be used with the config_store library.

*/

/*********************************/
/* GENERATED FILE - DO NOT EDIT! */
/*********************************/

#ifndef _SINK_BUTTON_CONFIG_DEF_H_
#define _SINK_BUTTON_CONFIG_DEF_H_

#include "config_definition.h"

typedef struct {
    unsigned short pio_mask;
    unsigned short chg_enabled:1;
    unsigned short vreg_enabled:1;
    unsigned short state_mask:14;
    unsigned short type:8;
    unsigned short user_event:8;
} event_config_type_t;

typedef struct {
    unsigned short input_number:8;
    unsigned short button_no:8;
} button_translation_type_t;

typedef struct {
    unsigned short event;
    unsigned short pattern[6];
} button_pattern_config_type_t;

typedef struct {
    unsigned short double_press_time;
    unsigned short long_press_time;
    unsigned short very_long_press_time;
    unsigned short repeat_time;
    unsigned short very_very_long_press_time;
    unsigned short debounce_period_ms:8;
    unsigned short debounce_number:8;
} button_config_type_t;

#define SINK_BUTTON_PATTERN_CONFIG_BLK_ID 150

typedef struct {
    button_pattern_config_type_t button_pattern[4];
} sink_button_pattern_config_def_t;

#define SINK_BUTTON_CONFIG_BLK_ID 182

typedef struct {
    button_config_type_t button_config;
    unsigned short padding:12;
    unsigned short IgnoreButtonPressAfterLedEnable:1;
    unsigned short separate_vd_buttons:1;
    unsigned short separate_lnr_buttons:1;
    unsigned short go_connectable_on_button_press:1;
} sink_button_config_def_t;

#define SINK_BUTTON_TRANSLATION_CONFIG_BLK_ID 193

typedef struct {
    button_translation_type_t button_translation[18];
} sink_button_translation_config_def_t;

#define SINK_BUTTON_EVENTSET_C_CONFIG_BLK_ID 215

typedef struct {
    event_config_type_t user_event_set_c[22];
} sink_button_eventset_c_config_def_t;

#define SINK_BUTTON_EVENTSET_B_CONFIG_BLK_ID 285

typedef struct {
    event_config_type_t user_event_set_b[22];
} sink_button_eventset_b_config_def_t;

#define SINK_BUTTON_EVENTSET_A_CONFIG_BLK_ID 355

typedef struct {
    event_config_type_t user_event_set_a[22];
} sink_button_eventset_a_config_def_t;

#endif /* _SINK_BUTTON_CONFIG_DEF_H_ */
