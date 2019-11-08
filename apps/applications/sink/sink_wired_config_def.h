
/****************************************************************************
Copyright (c) 2019 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_wired_config_def.h

DESCRIPTION 
    This file contains the module specific configuration structure(s) and
    configuration block id(s) to be used with the config_store library.

*/

/*********************************/
/* GENERATED FILE - DO NOT EDIT! */
/*********************************/

#ifndef _SINK_WIRED_CONFIG_DEF_H_
#define _SINK_WIRED_CONFIG_DEF_H_

#include "config_definition.h"

typedef struct {
    unsigned short word;
} i2s_config_t;

typedef struct {
    unsigned short volume_no_of_bits;
    unsigned short volume_range_max;
    unsigned short volume_range_min;
    unsigned short music_resampling_frequency;
    unsigned short voice_resampling_frequency;
    unsigned short bit_clock_scaling_factor;
    unsigned short master_clock_scaling_factor;
    unsigned short master_operation:8;
    unsigned short plugin_type:8;
    unsigned short i2s_configuration_command_pskey_length:8;
    unsigned short bits_per_sample:8;
    unsigned short number_of_volume_cmds:8;
    unsigned short number_of_initialisation_cmds:8;
    unsigned short number_of_shutdown_cmds:8;
    unsigned short volume_cmds_offset:8;
    unsigned short enable_pio:8;
    unsigned short shutdown_cmds_offset:8;
    unsigned short padding:7;
    unsigned short enable_24_bit_audio_input:1;
    unsigned short justified_bit_delay:4;
    unsigned short left_or_right_justified:4;
} i2s_init_config_t_t;

typedef struct {
    unsigned short padding:7;
    unsigned short enable_24_bit_audio:1;
    unsigned short target_latency:8;
} spdif_configuration_t;

typedef struct {
    signed short main_volume;
    signed short aux_volume;
} volume_info_t;

#define SINK_WIRED_WRITEABLE_CONFIG_BLK_ID 1073

typedef struct {
    volume_info_t analog_volume;
    volume_info_t spdif_volume;
    volume_info_t i2s_volume;
} sink_wired_writeable_config_def_t;

#define SINK_WIRED_READONLY_CONFIG_BLK_ID 1083

typedef struct {
    unsigned short WiredAudioConnectedPowerOffTimeout_s;
    unsigned short AnalogueAudioDisconnectDebounce_ms;
    unsigned short wired_audio_output_rate:8;
    unsigned short wired_audio_input_rate:8;
    unsigned short analog_input:8;
    unsigned short spdif_detect:8;
    unsigned short i2s_detect:8;
    unsigned short spdif_input:8;
    unsigned short padding:2;
    unsigned short PlayUsbWiredInLimbo:1;
    unsigned short PowerOffOnWiredAudioConnected:1;
    unsigned short spdif_audio_instance:2;
    unsigned short i2s_audio_instance:2;
    unsigned short spdif_output:8;
} sink_wired_readonly_config_def_t;

#define SPDIF_SETTINGS_CONFIG_BLK_ID 1093

typedef struct {
    spdif_configuration_t SpdifConfig;
} spdif_settings_config_def_t;

#define I2S_DATA_CONFIG_BLK_ID 1098

typedef struct {
    i2s_init_config_t_t i2s_init_config;
    i2s_config_t data[1];
} i2s_data_config_def_t;

#endif /* _SINK_WIRED_CONFIG_DEF_H_ */
